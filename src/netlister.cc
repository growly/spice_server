#include "netlister.h"

#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include <Python.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_join.h>

#include "proto/spice_simulator.pb.h"

DEFINE_string(python_vlsirtools, "../vlsir_repo/VlsirTools",
              "Path to root of vlsirtools python module.");
DEFINE_string(python_vlsir, "../vlsir_repo/bindings/python",
              "Path to generated python vlsir bindings.");

namespace spiceserver {

void Netlister::InitialisePython() {
  LOG(INFO) << "Starting Python";
  PyStatus py_status;
  PyConfig_InitPythonConfig(&py_config_);
  py_status = Py_InitializeFromConfig(&py_config_);
  if (PyStatus_Exception(py_status)) {
    LOG(ERROR) << "Could not initialise python from config";
  }
}

void Netlister::ConfigurePythonPostInit() {
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
}

void Netlister::NewConfiguredPythonInterpreter() {
  DCHECK(py_thread_state_ == nullptr);
  py_thread_state_ = Py_NewInterpreter();
  ConfigurePythonPostInit();
}

void Netlister::EndPythonInterpreter() {
  Py_EndInterpreter(py_thread_state_);
  py_thread_state_ = nullptr;
}

void Netlister::DeinitialisePython() {
  LOG(INFO) << "Shutting down Python";
  PyConfig_Clear(&py_config_);
  Py_FinalizeEx();
}

// Calling Python directly vs. spawning a subprocess
// =================================================
//
// It is both tempting and "neat" (as in "neato") to link against python
// libraries already installed on the system and call the interpreter through
// its API.
//
// But some python libraries (*COUGH* numpy *COUGH*), which have their
// own DLLs, do not behave well when being unloaded/loaded/reloaded between
// successive invocations of the interpreter. As a 
//
// Run python3-config --cflags to figure out where this is, or (better) use the
// cmake Python package to locate it for the build:
//
// #include <python3.11/Python.h>

std::vector<std::filesystem::path> Netlister::WriteSim(
    const vlsir::spice::SimInput &sim_input_pb,
    const Flavour &spice_flavour,
    const std::filesystem::path &output_directory) {
  // vlsirtools/spice contains classes that implement the conversion of
  // SimInput parameters to specific simulator commands (for including in the
  // top level deck). TODO(aryap):
  //  1) separate the logic which writes the top-level deck from running the
  //  exectuable on them
  //  2) hide this behind the existing interface so as to not break any other
  //  code (no mood for fixing those yet)
  //  3) write_sim_input seems to be the version of (1) that alrady exists, but
  //  it has missing parts? it works for SpectreNetlister.
  //  4) ok our job is to complete Netlister.write_sim_input!
  std::string file_name = (output_directory / std::filesystem::path(
      "vlsir_sim_input.pb")).string();
  std::fstream output_file(
      file_name, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!sim_input_pb.SerializeToOstream(&output_file)) {
    LOG(ERROR) << "Could not write input proto to " << file_name;
    return {};
  } else {
    LOG(INFO) << "SimInput protobuf written to " << file_name;
  }

  std::filesystem::path out_file_name = output_directory / "main.sp";

  std::string python_script = absl::StrCat("input_pb_name = '", file_name, "'\n");
  python_script += absl::StrCat(
      "out_file_name = '", out_file_name.string(), "'\n");
  python_script +=
      "#import sys\n"
      "#print(sys.path)\n"
      "import vlsir.spice_pb2 as spice_pb2\n"
      "from vlsirtools.netlist.spice import XyceNetlister\n"
      "sim_input_pb = spice_pb2.SimInput()\n"
      "with open(input_pb_name, 'rb') as in_file:\n"
      "  print('reading:', input_pb_name)\n"
      "  sim_input_pb.ParseFromString(in_file.read())\n"
      "with open(out_file_name, 'w') as out_file:\n"
      "  print('writing:', out_file_name)\n"
      "  netlister = XyceNetlister(out_file)\n"
      "  netlister.write_sim_input(sim_input_pb)\n";


  VLOG(11) << "Running script:\n" << python_script;

  PyRun_SimpleString(python_script.c_str());

  // TODO(aryap): Need to extract any errors from running Python script.

  return {out_file_name};
}

std::vector<std::filesystem::path> Netlister::WriteSpice(
    const vlsir::circuit::Package &circuit_pb,
    const Flavour &spice_flavour,
    const std::filesystem::path &output_directory) {
  std::string file_name = (output_directory / std::filesystem::path(
      "vlsir_package.pb")).string();
  std::fstream output_file(
      file_name, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!circuit_pb.SerializeToOstream(&output_file)) {
    LOG(ERROR) << "Could not write input proto to " << file_name;
    return {};
  } else {
    LOG(INFO) << "Circuit protobuf written to " << file_name;
  }

  std::filesystem::path out_file_name = output_directory / "netlist.sp";

  std::string python_script = absl::StrCat("input_pb_name = '", file_name, "'\n");
  python_script += absl::StrCat(
      "out_file_name = '", out_file_name.string(), "'\n");
  python_script +=
      "#import sys\n"
      "#print(sys.path)\n"
      "import vlsir.circuit_pb2 as circuit_pb2\n"
      "from vlsirtools.netlist.spice import XyceNetlister\n"
      "package_pb = circuit_pb2.Package()\n"
      "with open(input_pb_name, 'rb') as in_file:\n"
      "  print('reading:', input_pb_name)\n"
      "  package_pb.ParseFromString(in_file.read())\n"
      "with open(out_file_name, 'w') as out_file:\n"
      "  print('writing:', out_file_name)\n"
      "  netlister = XyceNetlister(out_file)\n"
      "  netlister.write_package(package_pb)\n";


  VLOG(11) << "Running script:\n" << python_script;

  PyRun_SimpleString(python_script.c_str());

  // TODO(aryap): Need to extract any errors from running Python script.

  return {out_file_name};
}

}  // namespace spiceserver
