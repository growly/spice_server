cmake_minimum_required(VERSION 3.15)
project(SpiceServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# If you can get the cmake protobuf package generation function to
# simultaneously:
# - include vlsir protos in the path for spice server protos;
# - generate C++ files with include paths that point to where the files
# actually went; and
# - not have generate name symbol name conflicts, since these also depend on
# the generating path;
# then please fix this shit and tell me how you did it.
#
# The current problem is that proto/spice_simulator.proto can find
# vlsir/spice.proto, but when building vlsir/*.proto files, symbols do not
# include a "vlsir::" prefix. It looks like "utils.proto" types are given the
# "vlsir::" prefix but this isn't included unless that files is included with
# the "vlsir/" in front of it in the import path.

get_filename_component(VLSIR_PROTO_DIR "vlsir" ABSOLUTE)
file(GLOB_RECURSE VLSIR_PROTO_SRC "${VLSIR_PROTO_DIR}/*.proto")
file(GLOB_RECURSE SPICE_SERVER_PROTO_SRC "proto/*.proto")

foreach(_file ${SPICE_SERVER_PROTO_SRC})
  message(STATUS "proto file: " ${_file})
endforeach()
foreach(_file ${VLSIR_PROTO_SRC})
  message(STATUS "proto file: " ${_file})
endforeach()

# Generate protobuf and gRPC code
set(GENERATED_VLSIR_SRC)
protobuf_generate(
  LANGUAGE cpp
  PROTOS ${VLSIR_PROTO_SRC}
  OUT_VAR GENERATED_VLSIR_SRC
  PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
  APPEND_PATH OFF
)
set(GENERATED_PROTO_SRC)
protobuf_generate(
  LANGUAGE cpp
  PROTOS ${SPICE_SERVER_PROTO_SRC}
  OUT_VAR GENERATED_PROTO_SRC
  PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
  APPEND_PATH OFF
  IMPORT_DIRS "vlsir"
)
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
set(GENERATED_GRPC_SRC)
protobuf_generate(
  LANGUAGE grpc
  PROTOS ${SPICE_SERVER_PROTO_SRC}
  GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
  PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
  OUT_VAR GENERATED_GRPC_SRC
  PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
  IMPORT_DIRS "vlsir"
)

set(GENERATED_ALL_SRC
  ${GENERATED_VLSIR_SRC}
  ${GENERATED_PROTO_SRC}
  ${GENERATED_GRPC_SRC}
)

# Generate C++ code from proto files
add_library(spice_proto ${GENERATED_ALL_SRC})
target_link_libraries(spice_proto
  PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

# Main executable
add_executable(spice_server
  src/main.cc
  src/simulator_service.cc
  src/simulator_manager.cc
  src/simulator_registry.cc
)

target_include_directories(
  spice_proto
  PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}/vlsir
)

target_include_directories(
  spice_server
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/proto
    ${CMAKE_CURRENT_BINARY_DIR}/vlsir
)

target_link_libraries(spice_server
  PRIVATE
    spice_proto
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
    Threads::Threads
)
