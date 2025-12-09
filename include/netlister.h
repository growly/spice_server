#ifndef NETLISTER_H_
#define NETLISTER_H_

#include <sys/types.h>
#include <functional>
#include <filesystem>
#include <string>
#include <vector>

#include <Python.h>
#include <absl/status/statusor.h>

#include "simulator_registry.h"
#include "proto/spice_simulator.pb.h"
#include "vlsir/spice.pb.h"

// 
// We use a singleton so that we can can cleanly initialise the python
// interpreter and also destroy it at the end of the program. (It turns out
// that it's not possible to correctly initialize and de-initialize it when
// using some modules, like numpy.)

namespace spiceserver {

class Netlister {
 public:
  Netlister(const Netlister &other) = delete;
  Netlister &operator=(const Netlister &other) = delete;

  static Netlister &GetInstance() {
    static Netlister instance;
    return instance;
  }

  // Write the given VLSIR package to disk at the given output_directory, in
  // the given spice flavour. Returns a list of created files, with the file
  // corresponding to the top-level module first.
  std::vector<std::filesystem::path> WriteSim(
      const vlsir::spice::SimInput &sim_input_pb,
      const Flavour &spice_flavour,
      const std::filesystem::path &output_directory);

  std::vector<std::filesystem::path> WriteSpice(
      const vlsir::circuit::Package &circuit_pb,
      const Flavour &spice_flavour,
      const std::filesystem::path &output_directory);

 private:
  Netlister()
    : py_thread_state_(nullptr) {
    InitialisePython();
    ConfigurePythonPostInit();
  }
  ~Netlister() {
    DeinitialisePython();
  }

  // FIXME(aryap): numpy is bad at being run in a Python init/de-init loop or
  // in sub-interpreters. It even says:
  //
  // UserWarning: NumPy was imported from a Python sub-interpreter but NumPy
  // does not properly support sub-interpreters. This will likely work for most
  // users but might cause hard to track down issues or subtle bugs. A common
  // user of the rare sub-interpreter feature is wsgi which also allows
  // single-interpreter mode. Improvements in the case of bugs are welcome,
  // but is not on the NumPy roadmap, and full support may require significant
  // effort to achieve.
  PyThreadState *py_thread_state_;
  void NewConfiguredPythonInterpreter();
  void EndPythonInterpreter();

  void ConfigurePythonPostInit();

  PyConfig py_config_;
  void InitialisePython();
  void DeinitialisePython();
};

} // namespace spiceserver

#endif // NETLISTER_H_
