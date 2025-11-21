#pragma once

#include <grpcpp/grpcpp.h>
#include "proto/spice_simulator.grpc.pb.h"

namespace spiceserver {

class SimulatorServiceImpl final : public SpiceSimulator::Service {
public:
    grpc::Status RunSimulation(
        grpc::ServerContext* context,
        const SimulationRequest* request,
        grpc::ServerWriter<SimulationResponse>* writer) override;

private:
    void StreamProcessOutput(
        int fd,
        SimulationResponse::StreamType stream_type,
        grpc::ServerWriter<SimulationResponse>* writer);
};

} // namespace spiceserver
