#ifndef SIMULATOR_REGISTRY_H_
#define SIMULATOR_REGISTRY_H_

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "proto/spice_simulator.pb.h"

namespace spiceserver {

// Registry for mapping Flavour enums to simulator executables and metadata
class SimulatorRegistry {
 public:
  struct SimulatorInfo {
    std::string path;
    std::string version;
    std::string name;
    std::string license;
    std::vector<Flavour> flavours;
  };

  SimulatorRegistry();
  ~SimulatorRegistry() = default;

  // Register a simulator with its path and metadata
  void RegisterSimulator(Flavour flavour, const SimulatorInfo& info);

  // Get the path for a given simulator flavour
  std::optional<std::string> GetSimulatorPath(Flavour flavour) const;

  // Get full info for a given simulator flavour
  std::optional<SimulatorInfo> GetSimulatorInfo(Flavour flavour) const;

  // Get all registered simulators
  std::vector<SimulatorInfo> GetAllSimulators() const;

  // Check if a simulator is registered
  bool IsRegistered(Flavour flavour) const;

 private:
  void RegisterDefaultSimulators();
  std::optional<std::string> FindExecutableInPath(
      const std::string& executable_name) const;

  std::map<Flavour, SimulatorInfo> simulators_;
};

}  // namespace spiceserver

#endif  // SIMULATOR_REGISTRY_H_
