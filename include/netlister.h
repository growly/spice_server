#ifndef NETLISTER_H_
#define NETLISTER_H_

#include <sys/types.h>
#include <functional>
#include <filesystem>
#include <string>
#include <vector>

#include <absl/status/statusor.h>

#include "simulator_registry.h"
#include "proto/spice_simulator.pb.h"
#include "vlsir/spice.pb.h"

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

class Netlister {
 public:
  
  // Write the given VLSIR package to disk at the given output_directory, in
  // the given spice flavour. Returns a list of created files, with the file
  // corresponding to the top-level module first.
  static std::vector<std::filesystem::path> WriteSim(
      const vlsir::spice::SimInput &sim_input_pb,
      const Flavour &spice_flavour,
      const std::filesystem::path &output_directory);
};

} // namespace spiceserver

#endif // NETLISTER_H_
