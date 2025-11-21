#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "simulator_service.h"

void RunServer(const std::string &server_address) {
    spiceserver::SimulatorServiceImpl service;

    grpc::ServerBuilder builder;
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "SpiceServer server listening on " << server_address
              << std::endl;

    server->Wait();
}

int main(int argc, char **argv) {
    std::string server_address = "0.0.0.0:50051";

    if (argc > 1) {
        server_address = argv[1];
    }

    std::cout << "Starting SpiceServer service..." << std::endl;
    RunServer(server_address);

    return 0;
}
