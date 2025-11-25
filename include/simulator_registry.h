#ifndef SIMULATOR_REGISTRY_H_
#define SIMULATOR_REGISTRY_H_

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <gflags/gflags.h>

#include "proto/spice_simulator.pb.h"

namespace spiceserver {

// Registry for all installed Spice simulators, their versions, paths, etc.
// This is a singleton.
class SimulatorRegistry {
 public:
  struct SimulatorInfo {
    std::string path;
    std::string version;
    std::string name;
    std::string license;
    std::vector<Flavour> flavours;
  };

  SimulatorRegistry(const SimulatorRegistry&) = delete;
  SimulatorRegistry& operator=(const SimulatorRegistry&) = delete;
  static SimulatorRegistry& GetInstance() {
    static SimulatorRegistry instance;
    return instance;
  }

  // Register a simulator with its path and metadata
  void RegisterSimulator(Flavour flavour, const SimulatorInfo& info);

  void RegisterSimulators(const StaticInstalls &static_installs_pb);

  // Get the path for a given simulator flavour
  std::optional<std::string> GetSimulatorPath(Flavour flavour) const;

  // Get full info for a given simulator flavour
  std::optional<SimulatorInfo> GetSimulatorInfo(Flavour flavour) const;

  // Get all registered simulators
  std::vector<SimulatorInfo> GetAllSimulators() const;

  // Check if a simulator is registered
  bool IsRegistered(Flavour flavour) const;

  std::string ReportInstalled() const;

 private:
  SimulatorRegistry();
  ~SimulatorRegistry() = default;

  void RegisterDefaultSimulators();
  std::optional<std::string> FindExecutableInPath(
      const std::string& executable_name) const;

  std::map<Flavour, SimulatorInfo> simulators_;
};

}  // namespace spiceserver

#endif  // SIMULATOR_REGISTRY_H_
