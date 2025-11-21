cmake_minimum_required(VERSION 3.15)
project(SpiceServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Proto file
set(PROTO_FILES
    proto/spice_simulator.proto
)

# Generate C++ code from proto files
add_library(spice_proto ${PROTO_FILES})
target_link_libraries(spice_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
)
target_include_directories(spice_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Generate protobuf and gRPC code
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET spice_proto LANGUAGE cpp)
protobuf_generate(TARGET spice_proto LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)

# Main executable
add_executable(spice_server
    src/main.cc
    src/simulator_service.cc
)

target_include_directories(spice_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(spice_server
    PRIVATE
        spice_proto
        gRPC::grpc++
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        Threads::Threads
)
