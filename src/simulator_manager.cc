#include "simulator_manager.h"

#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <array>
#include <cstring>
#include <glog/logging.h>
#include <vector>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_join.h>

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

absl::StatusOr<std::string> SimulatorManager::PrepareVerbatimInputsOnDisk(
    const std::vector<FileInfo> &files) {
  auto temp = CreateTemporaryDirectory();
  if (!temp.ok()) {
    return temp.status();
  }

  LOG(INFO) << "Using temp dir: " << *temp;
  for (const FileInfo &file_info_pb : files) {
    std::filesystem::path path(*temp);
    path /= std::filesystem::path(file_info_pb.path());
    LOG(INFO) << "Writing: " << path;

    std::filesystem::path directories = path;
    directories.remove_filename();

    std::filesystem::create_directories(directories);

    std::ofstream of;
    of.open(path, std::ios::out | std::ios::binary);
    of.write(file_info_pb.data().c_str(), file_info_pb.data().size());
    of.close();
  }

  return *temp;
}

absl::Status SimulatorManager::RunSimulator(
    const Flavour &flavour,
    const std::vector<FileInfo> &files,
    const std::vector<std::string> &additional_args) {
  auto simulator_info =
      SimulatorRegistry::GetInstance().GetSimulatorInfo(flavour);
  if (!simulator_info) {
    return absl::InvalidArgumentError("No simulator found.");
  }

  auto result_or = PrepareVerbatimInputsOnDisk(files);
  if (!result_or.ok()) {
    return result_or.status();
  }
  std::string directory = *result_or;

  std::vector<std::string> args(additional_args.begin(), additional_args.end());
  args.insert(args.begin(), files.begin()->path());

  const std::string &command = simulator_info->path;
  auto result = SpawnProcess(command, args, directory);
  if (!result.ok()) {
    return result;
  }

  return absl::OkStatus();
}

absl::Status SimulatorManager::SpawnProcess(
    const std::string &command,
    const std::vector<std::string> &args,
    const std::string &directory) {
  if (process_spawned_) {
    return absl::AlreadyExistsError("The process has already been spawned.");
  }

  // Create pipes for stdout and stderr. Index 0 gets the read end of the pipe,
  // and index 1 gets the write end.
  if (pipe(stdout_pipe_) == -1 || pipe(stderr_pipe_) == -1) {
    CleanupPipes();
    return absl::InternalError("Could not create pipes to child process.");
  }

  pid_ = fork();

  if (pid_ == -1) {
    CleanupPipes();
    return absl::InternalError("Fork failed.");
  }

  if (pid_ == 0) {
    std::filesystem::current_path(directory);

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

    // Execute the command.
    execvp(command.c_str(), argv.data());

    // If execvp returns, it failed.
    LOG(ERROR) << "Failed to execute command: " << strerror(errno);
    _exit(1);
  }

  std::vector<std::string> full_command(args.begin(), args.end());
  full_command.insert(full_command.begin(), command);
  LOG(INFO) << "Child is running command: " << absl::StrJoin(full_command, " ");

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

  return absl::OkStatus();
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

// TODO(aryap): We need a background process to expire (and delete) old
// temporary directories after some timeout.
absl::StatusOr<std::string> SimulatorManager::CreateTemporaryDirectory() {
  std::string template_str =
      std::filesystem::temp_directory_path().string() + "/spice_server.XXXXXX";
  // TODO(aryap): const_cast is bad juju! I'm not sure this is legal.
  char *directory_name = mkdtemp(const_cast<char*>(template_str.c_str()));
  if (directory_name == nullptr) {
    // Problem.
    return absl::UnavailableError("mkdtemp failed to make temporary directory");
  }
  // Is the char* going to leak?
  std::string name(directory_name);
  return name;
}

}  // namespace spiceserver
