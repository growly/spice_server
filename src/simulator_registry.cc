#include "simulator_registry.h"

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>

namespace spiceserver {

SimulatorRegistry::SimulatorRegistry() { RegisterDefaultSimulators(); }

void SimulatorRegistry::RegisterSimulator(Flavour flavour,
                                          const SimulatorInfo& info) {
  simulators_[flavour] = info;
}

std::optional<std::string> SimulatorRegistry::GetSimulatorPath(
    Flavour flavour) const {
  auto it = simulators_.find(flavour);
  if (it != simulators_.end()) {
    return it->second.path;
  }
  return std::nullopt;
}

std::optional<SimulatorRegistry::SimulatorInfo>
SimulatorRegistry::GetSimulatorInfo(Flavour flavour) const {
  auto it = simulators_.find(flavour);
  if (it != simulators_.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::vector<SimulatorRegistry::SimulatorInfo>
SimulatorRegistry::GetAllSimulators() const {
  std::vector<SimulatorInfo> result;
  result.reserve(simulators_.size());
  for (const auto& [flavour, info] : simulators_) {
    result.push_back(info);
  }
  return result;
}

bool SimulatorRegistry::IsRegistered(Flavour flavour) const {
  return simulators_.find(flavour) != simulators_.end();
}

std::optional<std::string> SimulatorRegistry::FindExecutableInPath(
    const std::string& executable_name) const {
  const char* path_env = std::getenv("PATH");
  if (!path_env) {
    return std::nullopt;
  }

  std::string path_str(path_env);
  std::istringstream path_stream(path_str);
  std::string dir;

  while (std::getline(path_stream, dir, ':')) {
    std::filesystem::path full_path =
        std::filesystem::path(dir) / executable_name;
    if (std::filesystem::exists(full_path) &&
        std::filesystem::is_regular_file(full_path)) {
      // Check if executable
      auto perms = std::filesystem::status(full_path).permissions();
      if ((perms & std::filesystem::perms::owner_exec) !=
              std::filesystem::perms::none ||
          (perms & std::filesystem::perms::group_exec) !=
              std::filesystem::perms::none ||
          (perms & std::filesystem::perms::others_exec) !=
              std::filesystem::perms::none) {
        return full_path.string();
      }
    }
  }

  return std::nullopt;
}

void SimulatorRegistry::RegisterDefaultSimulators() {
  // Try to find ngspice in PATH
  auto ngspice_path = FindExecutableInPath("ngspice");
  if (ngspice_path) {
    SimulatorInfo ngspice_info{
        .path = *ngspice_path,
        .version = "unknown",
        .name = "ngspice",
        .license = "BSD 3-Clause",
        .flavours = {Flavour::NGSPICE},
    };
    RegisterSimulator(Flavour::NGSPICE, ngspice_info);
  }

  // Try to find Xyce in PATH
  auto xyce_path = FindExecutableInPath("Xyce");
  if (xyce_path) {
    SimulatorInfo xyce_info{
        .path = *xyce_path,
        .version = "unknown",
        .name = "Xyce",
        .license = "GPL",
        .flavours = {Flavour::XYCE},
    };
    RegisterSimulator(Flavour::XYCE, xyce_info);
  }

  // Try to find hspice in PATH
  auto hspice_path = FindExecutableInPath("hspice");
  if (hspice_path) {
    SimulatorInfo hspice_info{
        .path = *hspice_path,
        .version = "unknown",
        .name = "HSPICE",
        .license = "Proprietary",
        .flavours = {Flavour::HSPICE},
    };
    RegisterSimulator(Flavour::HSPICE, hspice_info);
  }

  // Try to find spectre in PATH
  auto spectre_path = FindExecutableInPath("spectre");
  if (spectre_path) {
    SimulatorInfo spectre_info{
        .path = *spectre_path,
        .version = "unknown",
        .name = "Spectre",
        .license = "Proprietary",
        .flavours = {Flavour::SPECTRE},
    };
    RegisterSimulator(Flavour::SPECTRE, spectre_info);
  }

  // For version-specific Xyce entries, we can register them with the same
  // path if found, or look for version-specific installations
  // This is a placeholder - in a real implementation, you might check
  // specific installation directories or query version information
  if (xyce_path) {
    SimulatorInfo xyce_7_8_info{
        .path = *xyce_path,
        .version = "7.8",
        .name = "Xyce",
        .license = "GPL",
        .flavours = {Flavour::XYCE_7_8},
    };
    RegisterSimulator(Flavour::XYCE_7_8, xyce_7_8_info);

    SimulatorInfo xyce_7_9_info{
        .path = *xyce_path,
        .version = "7.9",
        .name = "Xyce",
        .license = "GPL",
        .flavours = {Flavour::XYCE_7_9},
    };
    RegisterSimulator(Flavour::XYCE_7_9, xyce_7_9_info);

    SimulatorInfo xyce_7_10_info{
        .path = *xyce_path,
        .version = "7.10",
        .name = "Xyce",
        .license = "GPL",
        .flavours = {Flavour::XYCE_7_10},
    };
    RegisterSimulator(Flavour::XYCE_7_10, xyce_7_10_info);
  }
}

}  // namespace spiceserver
