#include "simulator_manager.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <glog/logging.h>

#include <absl/status/status.h>
#include <absl/status/statusor.h>

#include "embedded_python_netlister.h"
#include "simulator_registry.h"
#include "subprocess.h"
#include "proto/spice_simulator.pb.h"

namespace spiceserver {

SimulatorManager::SimulatorManager() {}

SimulatorManager::~SimulatorManager() {}

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
  auto result = subprocess_.Spawn(command, args, directory);
  if (!result.ok()) {
    return result;
  }

  return absl::OkStatus();
}

absl::Status SimulatorManager::RunSimulator(
    const Flavour &flavour,
    const vlsir::spice::SimInput &sim_input,
    const std::vector<std::string> &additional_args) {
  auto simulator_info =
      SimulatorRegistry::GetInstance().GetSimulatorInfo(flavour);
  if (!simulator_info) {
    return absl::InvalidArgumentError("No simulator found.");
  }

  auto result_or = CreateTemporaryDirectory();
  if (!result_or.ok()) {
    return result_or.status();
  }
  std::filesystem::path directory(*result_or);

  auto netlists = EmbeddedPythonNetlister::GetInstance().WriteSim(
      sim_input, flavour, directory);
  if (netlists.empty()) {
    return absl::InvalidArgumentError(
        "Could not convert VLSIR SimInput to a SPICE netlist");
  }

  std::vector<std::string> args(additional_args.begin(), additional_args.end());
  args.insert(args.begin(), netlists.begin()->string());

  const std::string &command = simulator_info->path;
  auto result = subprocess_.Spawn(command, args, directory);
  if (!result.ok()) {
    return result;
  }

  return absl::OkStatus();
}

bool SimulatorManager::PollAndReadOutput(Subprocess::OutputCallback callback) {
  return subprocess_.PollAndReadOutput(callback);
}

int SimulatorManager::WaitForCompletion() {
  return subprocess_.WaitForCompletion();
}

bool SimulatorManager::IsRunning() const {
  return subprocess_.IsRunning();
}

// TODO(aryap): We need a background process to expire (and delete) old
// temporary directories after some timeout.
absl::StatusOr<std::string> SimulatorManager::CreateTemporaryDirectory() {
  std::string template_str =
      std::filesystem::temp_directory_path().string() + "/spice_server.XXXXXX";
  // TODO(aryap): const_cast is bad juju! I'm not sure this is legal.
  // mkdtemp will overwrite the template with generated value.
  char *result = mkdtemp(const_cast<char*>(template_str.c_str()));
  if (result == nullptr) {
    return absl::UnavailableError("mkdtemp failed to make temporary directory");
  }
  return template_str;
}

}  // namespace spiceserver
