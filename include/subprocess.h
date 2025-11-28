#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

#include <sys/types.h>
#include <functional>
#include <string>
#include <vector>

#include <absl/status/status.h>

namespace spiceserver {

class Subprocess {
 public:
  enum class StreamType {
    STDOUT,
    STDERR
  };

  using OutputCallback = std::function<void(
      const char* data,
      size_t length,
      StreamType stream_type)>;

  Subprocess();
  ~Subprocess();

  // Spawns a subprocess with the given command and arguments in the specified
  // directory. Returns true on success, false on failure.
  absl::Status Spawn(const std::string &command,
                     const std::vector<std::string> &args,
                     const std::string &directory);

  // Polls and reads from subprocess stdout/stderr, invoking the callback
  // for each chunk of data received.
  // Returns true while the process is running, false when complete.
  bool PollAndReadOutput(OutputCallback callback);

  // Waits for the subprocess to complete and returns the exit code.
  // Returns -1 if the process was terminated by a signal.
  int WaitForCompletion();

  // Returns true if a subprocess is currently running.
  bool IsRunning() const;

 private:
  void CleanupPipes();
  void SetNonBlocking(int fd);

  pid_t pid_;
  int stdout_pipe_[2];
  int stderr_pipe_[2];
  bool stdout_open_;
  bool stderr_open_;
  bool process_spawned_;
};

}  // namespace spiceserver

#endif  // SUBPROCESS_H_
