#include "simulator_service.h"
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <iostream>
#include <poll.h>

namespace spiceserver {

grpc::Status SimulatorServiceImpl::RunSimulation(
    grpc::ServerContext* context,
    const SimulationRequest* request,
    grpc::ServerWriter<SimulationResponse>* writer) {

    // Validate input
    if (request->simulator().empty()) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                          "Simulator name is required");
    }

    // Create pipes for stdout and stderr
    int stdout_pipe[2];
    int stderr_pipe[2];

    if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1) {
        return grpc::Status(grpc::StatusCode::INTERNAL,
                          "Failed to create pipes");
    }

    pid_t pid = fork();

    if (pid == -1) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        return grpc::Status(grpc::StatusCode::INTERNAL,
                          "Failed to fork process");
    }

    if (pid == 0) {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        // Redirect stdout and stderr to pipes
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Build argument list
        std::vector<char*> args;
        args.push_back(const_cast<char*>(request->simulator().c_str()));

        for (const auto& arg : request->args()) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);

        // Execute the simulator
        execvp(request->simulator().c_str(), args.data());

        // If execvp returns, it failed
        std::cerr << "Failed to execute simulator: " << strerror(errno) << std::endl;
        _exit(1);
    }

    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    // Set pipes to non-blocking
    fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(stderr_pipe[0], F_SETFL, O_NONBLOCK);

    // Poll both pipes and stream output
    std::array<pollfd, 2> fds;
    fds[0].fd = stdout_pipe[0];
    fds[0].events = POLLIN;
    fds[1].fd = stderr_pipe[0];
    fds[1].events = POLLIN;

    char buffer[4096];
    bool stdout_open = true;
    bool stderr_open = true;

    while (stdout_open || stderr_open) {
        int poll_result = poll(fds.data(), 2, 1000);

        if (poll_result == -1) {
            break;
        }

        // Read from stdout
        if (stdout_open && (fds[0].revents & POLLIN)) {
            ssize_t count = read(stdout_pipe[0], buffer, sizeof(buffer) - 1);
            if (count > 0) {
                buffer[count] = '\0';
                SimulationResponse response;
                response.set_output(buffer, count);
                response.set_stream_type(SimulationResponse::STDOUT);
                response.set_done(false);
                writer->Write(response);
            } else if (count == 0) {
                stdout_open = false;
            }
        }

        // Read from stderr
        if (stderr_open && (fds[1].revents & POLLIN)) {
            ssize_t count = read(stderr_pipe[0], buffer, sizeof(buffer) - 1);
            if (count > 0) {
                buffer[count] = '\0';
                SimulationResponse response;
                response.set_output(buffer, count);
                response.set_stream_type(SimulationResponse::STDERR);
                response.set_done(false);
                writer->Write(response);
            } else if (count == 0) {
                stderr_open = false;
            }
        }

        // Check for EOF
        if (fds[0].revents & POLLHUP) stdout_open = false;
        if (fds[1].revents & POLLHUP) stderr_open = false;
    }

    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    // Wait for child process to finish
    int status;
    waitpid(pid, &status, 0);

    // Send final message with exit code
    SimulationResponse final_response;
    final_response.set_done(true);

    if (WIFEXITED(status)) {
        final_response.set_exit_code(WEXITSTATUS(status));
    } else {
        final_response.set_exit_code(-1);
    }

    writer->Write(final_response);

    return grpc::Status::OK;
}

} // namespace spiceserver
