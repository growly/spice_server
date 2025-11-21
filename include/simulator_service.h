#ifndef SIMULATOR_SERVICE_H_
#define SIMULATOR_SERVICE_H_

#include <grpcpp/grpcpp.h>
#include "proto/spice_simulator.grpc.pb.h"

namespace spiceserver {

class SimulatorServiceImpl final : public SpiceSimulator::Service {
 public:
  grpc::Status RunSimulation(
      grpc::ServerContext* context,
      const SimulationRequest* request,
      grpc::ServerWriter<SimulationResponse>* writer) override;

  grpc::Status ListSimulators(
      grpc::ServerContext *context,
      const ListSimulatorsRequest *request,
      ListSimulatorsResponse *response) override;

 private:
  void StreamProcessOutput(
      int fd,
      SimulationResponse::StreamType stream_type,
      grpc::ServerWriter<SimulationResponse> *writer);
};

} // namespace spiceserver

#endif // SIMULATOR_SERVICE_H_
