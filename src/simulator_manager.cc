#include "simulator_manager.h"

#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>
#include <vector>

#include "simulator_registry.h"
#include "proto/spice_simulator.pb.h"

namespace spiceserver {

SimulatorManager::SimulatorManager()
    : pid_(-1),
      stdout_open_(false),
      stderr_open_(false),
      process_spawned_(false) {
  stdout_pipe_[0] = -1;
  stdout_pipe_[1] = -1;
  stderr_pipe_[0] = -1;
  stderr_pipe_[1] = -1;
}

SimulatorManager::~SimulatorManager() { CleanupPipes(); }

void SimulatorManager::CleanupPipes() {
  if (stdout_pipe_[0] != -1) {
    close(stdout_pipe_[0]);
    stdout_pipe_[0] = -1;
  }
  if (stdout_pipe_[1] != -1) {
    close(stdout_pipe_[1]);
    stdout_pipe_[1] = -1;
  }
  if (stderr_pipe_[0] != -1) {
    close(stderr_pipe_[0]);
    stderr_pipe_[0] = -1;
  }
  if (stderr_pipe_[1] != -1) {
    close(stderr_pipe_[1]);
    stderr_pipe_[1] = -1;
  }
}

void SimulatorManager::SetNonBlocking(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
}

bool SimulatorManager::RunSimulator(
    const Flavour &flavour,
    const std::vector<FileInfo> &files,
    const std::vector<std::string> &additional_args) {

  SimulatorRegistry this_should_be_singleton;

  auto simulator_info = this_should_be_singleton.GetSimulatorInfo(flavour);

  return true;
}

bool SimulatorManager::SpawnProcess(const std::string& command,
                                    const std::vector<std::string>& args) {
  if (process_spawned_) {
    return false;
  }

  // Create pipes for stdout and stderr. Index 0 gets the read end of the pipe,
  // and index 1 gets the write end.
  if (pipe(stdout_pipe_) == -1 || pipe(stderr_pipe_) == -1) {
    CleanupPipes();
    return false;
  }

  pid_ = fork();

  if (pid_ == -1) {
    CleanupPipes();
    return false;
  }

  if (pid_ == 0) {
    // Child process
    close(stdout_pipe_[0]);
    close(stderr_pipe_[0]);

    // Redirect stdout and stderr to pipes
    dup2(stdout_pipe_[1], STDOUT_FILENO);
    dup2(stderr_pipe_[1], STDERR_FILENO);

    close(stdout_pipe_[1]);
    close(stderr_pipe_[1]);

    // Build argument list
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(command.c_str()));

    for (const auto& arg : args) {
      argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    // Execute the command
    execvp(command.c_str(), argv.data());

    // If execvp returns, it failed
    std::cerr << "Failed to execute command: " << strerror(errno) << std::endl;
    _exit(1);
  }

  // Parent process
  close(stdout_pipe_[1]);
  stdout_pipe_[1] = -1;
  close(stderr_pipe_[1]);
  stderr_pipe_[1] = -1;

  // Set pipes to non-blocking
  SetNonBlocking(stdout_pipe_[0]);
  SetNonBlocking(stderr_pipe_[0]);

  stdout_open_ = true;
  stderr_open_ = true;
  process_spawned_ = true;

  return true;
}

bool SimulatorManager::PollAndReadOutput(OutputCallback callback) {
  if (!process_spawned_ || (!stdout_open_ && !stderr_open_)) {
    return false;
  }

  std::array<pollfd, 2> fds;
  fds[0].fd = stdout_pipe_[0];
  fds[0].events = POLLIN;
  fds[1].fd = stderr_pipe_[0];
  fds[1].events = POLLIN;

  char buffer[4096];

  int poll_result = poll(fds.data(), 2, 1000);

  if (poll_result == -1) {
    return false;
  }

  // Read from stdout
  if (stdout_open_ && (fds[0].revents & POLLIN)) {
    ssize_t count = read(stdout_pipe_[0], buffer, sizeof(buffer) - 1);
    if (count > 0) {
      callback(buffer, count, StreamType::STDOUT);
    } else if (count == 0) {
      stdout_open_ = false;
    }
  }

  // Read from stderr
  if (stderr_open_ && (fds[1].revents & POLLIN)) {
    ssize_t count = read(stderr_pipe_[0], buffer, sizeof(buffer) - 1);
    if (count > 0) {
      callback(buffer, count, StreamType::STDERR);
    } else if (count == 0) {
      stderr_open_ = false;
    }
  }

  // Check for EOF
  if (fds[0].revents & POLLHUP) stdout_open_ = false;
  if (fds[1].revents & POLLHUP) stderr_open_ = false;

  return stdout_open_ || stderr_open_;
}

int SimulatorManager::WaitForCompletion() {
  if (!process_spawned_) {
    return -1;
  }

  // Make sure pipes are closed before waiting
  if (stdout_pipe_[0] != -1) {
    close(stdout_pipe_[0]);
    stdout_pipe_[0] = -1;
  }
  if (stderr_pipe_[0] != -1) {
    close(stderr_pipe_[0]);
    stderr_pipe_[0] = -1;
  }

  int status;
  waitpid(pid_, &status, 0);

  process_spawned_ = false;

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  return -1;
}

bool SimulatorManager::IsRunning() const { return process_spawned_; }

}  // namespace spiceserver
