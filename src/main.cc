#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <memory>
#include <string>

#include <absl/strings/str_cat.h>

#include "utility.h"

#include "netlister.h"
#include "simulator_service.h"
#include "simulator_registry.h"
#include "proto/spice_simulator.pb.h"

// Define command line flags
DEFINE_string(static_installs,
              "",
              "Path to text-format StaticInstall text proto defining "
              "installed simulators.");
DEFINE_string(port, "50051", "Listen port");

void RunServer(const std::string &server_address) {
  spiceserver::SimulatorServiceImpl service;

  grpc::ServerBuilder builder;
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "SpiceServer server listening on " << server_address;

  server->Wait();
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;

  spiceserver::SimulatorRegistry &registry =
      spiceserver::SimulatorRegistry::GetInstance();

  if (!FLAGS_static_installs.empty()) {
    LOG(INFO) << "Using simulator static installs file: "
              << FLAGS_static_installs;

    spiceserver::StaticInstalls static_installs_pb;
    spiceserver::Utility::ReadTextProtoOrDie(
        FLAGS_static_installs, &static_installs_pb);

    registry.RegisterSimulators(static_installs_pb);
  }

  LOG(INFO) << "Installed: " << std::endl << registry.ReportInstalled();

  std::string server_address = absl::StrCat("0.0.0.0:", FLAGS_port);

  LOG(INFO) << "Starting SpiceServer service...";
  RunServer(server_address);

  return 0;
}
