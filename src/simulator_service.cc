#include "simulator_service.h"

#include <iostream>
#include <string>
#include <vector>

#include <absl/strings/str_cat.h>

#include "simulator_manager.h"

namespace spiceserver {

grpc::Status SimulatorServiceImpl::ListSimulators(
    grpc::ServerContext* context,
    const ListSimulatorsRequest* request,
    ListSimulatorsResponse* reseponse) {
  // Query the singleton/static SimulatorRegistry.

  return grpc::Status::OK;
}

grpc::Status SimulatorServiceImpl::RunSimulation(
    grpc::ServerContext* context, const SimulationRequest* request,
    grpc::ServerWriter<SimulationResponse>* writer) {
  // Validate input
  if (request->simulator() == Flavour::UNSET) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Simulator flavour is required");
  }

  SimulatorManager simulator_manager;

  if (request->has_vlsir_sim_input()) {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not today");
  } else if (request->has_verbatim_files()) {
    std::vector<FileInfo> file_infos(
        request->verbatim_files().files().begin(),
        request->verbatim_files().files().end());
    std::vector<std::string> additional_args(
        request->additional_args().begin(),
        request->additional_args().end());

    auto status = simulator_manager.RunSimulator(
        request->simulator(), file_infos, additional_args);
    if (!status.ok()) {
      std::string new_message = absl::StrCat(
          "Failed to spawn simulator process. ",
          status.message());
      return grpc::Status(grpc::StatusCode::INTERNAL, new_message);
    }
  } else {
    return grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT, "No circuit inputs.");
  }

  // Poll and stream output from the subprocess
  auto output_callback = [writer](const char* data, size_t length,
                                  SimulatorManager::StreamType stream_type) {
    SimulationResponse response;
    response.set_output(data, length);

    if (stream_type == SimulatorManager::StreamType::STDOUT) {
      response.set_stream_type(SimulationResponse::STDOUT);
    } else {
      response.set_stream_type(SimulationResponse::STDERR);
    }

    response.set_done(false);
    writer->Write(response);
  };

  while (simulator_manager.PollAndReadOutput(output_callback)) {
    // Continue polling while process is running and pipes are open
  }

  // Wait for subprocess to complete and get exit code
  int exit_code = simulator_manager.WaitForCompletion();

  // Send final message with exit code
  SimulationResponse final_response;
  final_response.set_done(true);
  final_response.set_exit_code(exit_code);
  writer->Write(final_response);

  return grpc::Status::OK;
}

}  // namespace spiceserver
