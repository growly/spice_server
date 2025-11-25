cmake_minimum_required(VERSION 3.15)
project(SpiceServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)
find_package(absl REQUIRED)

# VLSIR integration
# -----------------

# The central problem we face when integrating VLSIR protobufs is that they all
# reference on another with imports relative to the root of the proto
# directory, but in all cases where they are imported, they are kept in a
# separate (typically "vlsir") directory. protoc generates symbols with
# prefixes that seem to depend on the import path.
#
# We have to align paths and names for:
#   1) the .proto files and their 'import' statements;
#   2) the the generated C/C++ files and their '#include' statements; and
#   3) the prefixes in names of symbols the linker will need to search for.
#
# Concretely, if you import "./vlsir/spice.proto" with -I. as the import path
# to protoc, you get a very different set of symbol names to if you import
# "spice.proto" and pass -Ivlsir to protoc.

# For still-elusive reasons, using the protobuf cmake package's generation
# doesn't allow us enough control to manage all three at once, since it deduces
# output paths relatively. You can pass it import dirs, but you can't control
# where the output goes finely enough. That is all I can say about this after
# several hours fighting for my life. The following custom commands (borrowed
# from BFG) allow us to invoke protoc independently controlling include
# directories (1) and output directories (2), so that the symbol names are
# correct (3) and the files are in the right places.

function(get_unique_abs_paths_among out_var files)
  foreach(DIR ${files})
    get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
    list(FIND _unique_dirs ${ABS_PATH} _contains_already)
    if(${_contains_already} EQUAL -1)
        list(APPEND _unique_dirs --proto_path=${ABS_PATH})
    endif()
  endforeach()
  set("${out_var}" "${_unique_dirs}" PARENT_SCOPE)
endfunction()

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
set(GENERATED_GRPC_HDRS)
set(GENERATED_GRPC_SRCS)
function(generate_grpc)
  set(_options)
  set(_singleargs PROTO_SRC_ROOT OUT_DIR)
  set(_multiargs PROTO_FILES IMPORT_DIRS)
  cmake_parse_arguments(generate_grpc "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  get_unique_abs_paths_among(_protobuf_include_path "${generate_grpc_IMPORT_DIRS}")

  foreach(file ${generate_grpc_PROTO_FILES})
    message(STATUS "Generating gRPC C++ service from: ${file} -> ${generate_grpc_OUT_DIR}")
    file(RELATIVE_PATH relative_file ${generate_grpc_PROTO_SRC_ROOT} ${file})
    get_filename_component(proto_dir ${relative_file} DIRECTORY)
    get_filename_component(proto_name ${relative_file} NAME_WE)
    set(proto_hdr ${generate_grpc_OUT_DIR}/${proto_dir}/${proto_name}.pb.h)
    set(proto_src ${generate_grpc_OUT_DIR}/${proto_dir}/${proto_name}.pb.cc)
    set(grpc_hdr ${generate_grpc_OUT_DIR}/${proto_dir}/${proto_name}.grpc.pb.h)
    set(grpc_src ${generate_grpc_OUT_DIR}/${proto_dir}/${proto_name}.grpc.pb.cc)
    message(STATUS \tfile=${file})
    message(STATUS \trelative_file=${relative_file})
    message(STATUS \tproto_src_root=${generate_grpc_PROTO_SRC_ROOT})
    message(STATUS \tproto_dir=${proto_dir})
    message(STATUS \tproto_hdr=${proto_hdr})
    message(STATUS \tproto_src=${proto_src})
    message(STATUS \tgrpc_hdr=${grpc_hdr})
    message(STATUS \tgrpc_src=${grpc_src})
    message(STATUS \tadditional_includes=${_protobuf_include_path})
    # VERBATIM might break stuff here. Try quoting args and removing.
    add_custom_command(
      OUTPUT "${proto_hdr}" "${proto_src}" "${grpc_hdr}" "${grpc_src}"
      COMMAND protobuf::protoc
      ARGS --grpc_out=${generate_grpc_OUT_DIR}
           --cpp_out=${generate_grpc_OUT_DIR}
           --proto_path=${generate_grpc_PROTO_SRC_ROOT}
            ${_protobuf_include_path}
            #--plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
            --plugin=protoc-gen-grpc=${grpc_cpp_plugin_location}
           "${file}"
      DEPENDS ${file} protobuf::protoc
      COMMENT "Generate gRPC C++ service/protocol buffers for ${file}"
      VERBATIM)
    list(APPEND GENERATED_GRPC_HDRS "${proto_hdr}")
    list(APPEND GENERATED_GRPC_SRCS "${proto_src}")
    list(APPEND GENERATED_GRPC_HDRS "${grpc_hdr}")
    list(APPEND GENERATED_GRPC_SRCS "${grpc_src}")
  endforeach()

  set(GENERATED_GRPC_HDRS ${GENERATED_GRPC_HDRS} PARENT_SCOPE)
  set(GENERATED_GRPC_SRCS ${GENERATED_GRPC_SRCS} PARENT_SCOPE)
endfunction()

set(GENERATED_PROTO_SRCS)
set(GENERATED_PROTO_HDRS)
function(generate_protos)
  set(_options)
  set(_singleargs PROTO_SRC_ROOT OUT_DIR)
  set(_multiargs PROTO_FILES IMPORT_DIRS)
  cmake_parse_arguments(generate_protos "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  get_unique_abs_paths_among(_protobuf_include_path "${generate_protos_IMPORT_DIRS}")

  foreach(file ${generate_protos_PROTO_FILES})
    message(STATUS "Generating C++ proto from: ${file} -> ${generate_protos_OUT_DIR}")
    # Get the relative path of the proto to the source dir root.
    file(RELATIVE_PATH relative_file ${generate_protos_PROTO_SRC_ROOT} ${file})
    get_filename_component(proto_dir ${relative_file} DIRECTORY)
    get_filename_component(proto_name ${relative_file} NAME_WE)
    set(proto_hdr ${generate_protos_OUT_DIR}/${proto_dir}/${proto_name}.pb.h)
    set(proto_src ${generate_protos_OUT_DIR}/${proto_dir}/${proto_name}.pb.cc)
    message(STATUS \tfile=${file})
    message(STATUS \trelative_file=${relative_file})
    message(STATUS \tproto_dir=${proto_dir})
    message(STATUS \tproto_hdr=${proto_hdr})
    message(STATUS \tproto_src=${proto_src})
    message(STATUS \tadditional_includes=${_protobuf_include_path})
    add_custom_command(
      OUTPUT ${proto_hdr} ${proto_src}
      COMMAND protobuf::protoc
      "--proto_path=${generate_protos_PROTO_SRC_ROOT}"
      ${_protobuf_include_path}
      "--cpp_out=${generate_protos_OUT_DIR}"
      ${file}
      DEPENDS ${file} protobuf::protoc
      COMMENT "Generate C++ protocol buffer for ${file}"
      VERBATIM)
    list(APPEND GENERATED_PROTO_HDRS ${proto_hdr})
    list(APPEND GENERATED_PROTO_SRCS ${proto_src})
  endforeach()

  set(GENERATED_PROTO_HDRS ${GENERATED_PROTO_HDRS} PARENT_SCOPE)
  set(GENERATED_PROTO_SRCS ${GENERATED_PROTO_SRCS} PARENT_SCOPE)
endfunction()

# Set the VLSIR proto source directory and output directory, creating it if it
# doesn't exist. Generate protos into that output directory and save the list
# of generated files.
set(VLSIR_SRC_DIR ${PROJECT_SOURCE_DIR}/vlsir)
file(GLOB_RECURSE VLSIR_SRC "${VLSIR_SRC_DIR}/*.proto")
set(VLSIR_OUT_DIR ${PROJECT_BINARY_DIR}/vlsir)
file(MAKE_DIRECTORY ${VLSIR_OUT_DIR})
generate_protos(
  PROTO_SRC_ROOT ${VLSIR_SRC_DIR}
  PROTO_FILES "${VLSIR_SRC}"
  OUT_DIR ${VLSIR_OUT_DIR})
set(VLSIR_GENERATED_HDRS ${GENERATED_PROTO_HDRS})
set(VLSIR_GENERATED_SRCS ${GENERATED_PROTO_SRCS})

set(PROTO_SRC_DIR ${PROJECT_SOURCE_DIR}/proto)
file(GLOB_RECURSE PROTO_SRC "${PROTO_SRC_DIR}/*.proto")
set(PROTO_OUT_DIR ${PROJECT_BINARY_DIR}/proto)
file(MAKE_DIRECTORY ${PROTO_OUT_DIR})
set(GENERATED_GRPC_SRCS)
set(GENERATED_GRPC_HDRS)
# This will also make protos for definitions in the files with service
# definitions (the same .proto can't be attached to a custom build rule for a
# proto and grpc step at the same time).
generate_grpc(
  PROTO_SRC_ROOT ${PROTO_SRC_DIR}
  PROTO_FILES "${PROTO_SRC}"
  OUT_DIR ${PROTO_OUT_DIR}
  IMPORT_DIRS "${VLSIR_SRC_DIR}")
set(GRPC_GENERATED_HDRS ${GENERATED_GRPC_HDRS})
set(GRPC_GENERATED_SRCS ${GENERATED_GRPC_SRCS})

# Build all GRPC service files and proto files (for both ours and VLSIRs types)
# together.
add_library(proto_lib SHARED 
  ${VLSIR_GENERATED_SRCS}
  ${VLSIR_GENERATED_HDRS}
  ${GRPC_GENERATED_SRCS}
  ${GRPC_GENERATED_HDRS})
set_target_properties(proto_lib PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(proto_lib PRIVATE ${VLSIR_OUT_DIR})

# Main executable
add_executable(spice_server
  src/main.cc
  src/simulator_service.cc
  src/simulator_manager.cc
  src/simulator_registry.cc
)

target_include_directories(
  spice_server
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
    ${VLSIR_OUT_DIR}
    ${PROTO_OUT_DIR}
)

target_link_libraries(spice_server
  PRIVATE
    proto_lib
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
    Threads::Threads
    glog::glog
    gflags
    absl::strings
    absl::status
    absl::statusor
    absl::flat_hash_map
    absl::flat_hash_set
    absl::time
)
