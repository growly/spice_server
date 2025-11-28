#ifndef SIMULATOR_MANAGER_H_
#define SIMULATOR_MANAGER_H_

#include <string>
#include <vector>

#include <absl/status/statusor.h>

#include "simulator_registry.h"
#include "subprocess.h"
#include "proto/spice_simulator.pb.h"

// So you don't know how to do subprocess management and you just asked AI to
// write it all for you? Crazy enough to work. Here's what's happening at the
// system interface. We:
//
//  - ask the kernel for two new pipes, and for each it returns a file
//  descriptor for the read end and another for the write end
//  - fork the process
//    - in the child (the one that will run the simulator), tie stderr and
//    stdout to the write (input) end of those pipes we just made
//    - in the parent (the long-lived server), prepare to poll the read end of
//    those same pipes
//  - while the process runs, provide a mechanism to read the contents of each
//  buffer and give the data to some external function (callback)
//  - if we have to wait for completion, use another syscall to wait for the
//  child process to complete

namespace spiceserver {

class SimulatorManager {
 public:
  SimulatorManager();
  ~SimulatorManager();

  // Creates a temporary directory, writes the given files to disk, calls the
  // simulator (with any additional args). Results will then be available
  // through PollAndReadOutput.
  absl::Status RunSimulator(const Flavour &flavour,
                            const std::vector<FileInfo> &files,
                            const std::vector<std::string> &additional_args);

  // Same deal, but all netlist info is provided through VLSIR protobufs. This
  // requires an additional netlisting step, using a netlister appropriate to
  // the Spice flavour.
  // bool RunSimulator(const ...);

  // Polls and reads from subprocess stdout/stderr, invoking the callback
  // for each chunk of data received.
  // Returns true while the process is running, false when complete.
  bool PollAndReadOutput(Subprocess::OutputCallback callback);

  // Waits for the subprocess to complete and returns the exit code.
  // Returns -1 if the process was terminated by a signal.
  int WaitForCompletion();

  // Returns true if a subprocess is currently running.
  bool IsRunning() const;

 private:
  absl::StatusOr<std::string> PrepareVerbatimInputsOnDisk(
      const std::vector<FileInfo> &files);
  absl::StatusOr<std::string> CreateTemporaryDirectory();

  Subprocess subprocess_;
};

} // namespace spiceserver

#endif // SIMULATOR_MANAGER_H_
