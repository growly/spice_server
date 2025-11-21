#ifndef SIMULATOR_MANAGER_H_
#define SIMULATOR_MANAGER_H_

#include <sys/types.h>
#include <functional>
#include <string>
#include <vector>

#include "proto/spice_simulator.pb.h"

namespace spiceserver {

class SimulatorManager {
 public:
  enum class StreamType {
    STDOUT,
    STDERR
  };

  using OutputCallback = std::function<void(
      const char* data,
      size_t length,
      StreamType stream_type)>;

  SimulatorManager();
  ~SimulatorManager();

  // Creates a temporary directory, writes the given files to disk, calls the
  // simulator (with any additional args). Results will then be available
  // through PollAndReadOutput.
  bool RunSimulator(const Flavour &flavour,
                    const std::vector<FileInfo> &files,
                    const std::vector<std::string> &additional_args);

  // Same deal, but all netlist info is provided through VLSIR protobufs. This
  // requires an additional netlisting step, using a netlister appropriate to
  // the Spice flavour.
  // bool RunSimulator(const ...);

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
  // Spawns a subprocess with the given command and arguments.
  // Returns true on success, false on failure.
  bool SpawnProcess(const std::string &command,
                    const std::vector<std::string> &args);

  void CleanupPipes();
  void SetNonBlocking(int fd);

  pid_t pid_;
  int stdout_pipe_[2];
  int stderr_pipe_[2];
  bool stdout_open_;
  bool stderr_open_;
  bool process_spawned_;
};

} // namespace spiceserver

#endif // SIMULATOR_MANAGER_H_
