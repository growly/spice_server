#include "netlister.h"

#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

// Run python3.11-config --cflags to figure out where this is.
#include <python3.11/Python.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_join.h>

#include "proto/spice_simulator.pb.h"

DEFINE_string(python_vlsirtools, "../vlsir_repo/VlsirTools",
              "Path to root of vlsirtools python module.");
DEFINE_string(python_vlsir, "vlsir/",
              "Path to generated python vlsir bindings.");

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

  PyStatus py_status;
  PyConfig config;
  PyConfig_InitPythonConfig(&config);
  py_status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(py_status)) {
    LOG(ERROR) << "Could not initialise python from config";
  }

  PyObject *py_path_list = PySys_GetObject("path");
  if (!py_path_list) {
    LOG(ERROR) << "Could not get Python \"path\" object";
  }

  PyObject *py_vlsirtools_str = PyUnicode_FromString(
      FLAGS_python_vlsirtools.c_str());
  if (!py_vlsirtools_str) {
    LOG(ERROR) << "Could not make Python string for vlsirtools path";
  }
  PyList_Append(py_path_list, py_vlsirtools_str);

  PyObject *py_vlsir_str = PyUnicode_FromString(
      FLAGS_python_vlsir.c_str());
  if (!py_vlsir_str) {
    LOG(ERROR) << "Could not make Python string for vlsir path";
  }
  PyList_Append(py_path_list, py_vlsir_str);

  PyRun_SimpleString(
      "import sys\n"
      "print(sys.path)\n"
      "from vlsirtools.netlist.spice import XyceNetlister\n"
      "from time import time, ctime\n"
      "print('Today is',ctime(time()))\n"
  );
  Py_Finalize();
  
  return {};
}

}  // namespace spiceserver
