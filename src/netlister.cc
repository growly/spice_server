#include "netlister.h"

#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>

// Run python3.11-config --cflags to figure out where this is.
#include <python3.11/Python.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_join.h>

#include "proto/spice_simulator.pb.h"

DEFINE_string(vlsirtools, "../vlsir_repo/VlsirTools",
              "Path to root of vlsirtools python module.");

namespace spiceserver {

std::vector<std::filesystem::path> Netlister::WriteSim(
    const vlsir::spice::SimInput &sim_input_pb,
    const Flavour &spice_flavour,
    const std::filesystem::path &output_directory) {
  std::string file_name = (output_directory / std::filesystem::path(
      "sim_input.pb")).string();
  std::fstream output_file(
      file_name, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!sim_input_pb.SerializeToOstream(&output_file)) {
    LOG(ERROR) << "Failed to write SimInput message";
  } else {
    LOG(INFO) << "SimInput protobuf written to " << file_name;
  }

  Py_Initialize();
  PyRun_SimpleString(
      "from vlsirtools.netlist.spice import XyceNetlister\n"
      "from time import time, ctime\n"
      "print('Today is',ctime(time()))\n"
  );
  Py_Finalize();
  
  return {};
}

}  // namespace spiceserver
