# SpiceServer

This project was bootrapped by Claude, so keep that in mind.

A gRPC service for executing SPICE circuit simulations via various simulators as subprocesses.

## Overview

SpiceServer accepts gRPC requests containing SPICE netlists and executes them using the specified simulator (e.g., ngspice, Xyce, LTspice). Results are streamed back to the client in real-time.

## Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- gRPC and Protocol Buffers
- A SPICE simulator (ngspice, Xyce, etc.)

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y cmake build-essential
sudo apt-get install -y libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc
```

On macOS with Homebrew:
```bash
brew install cmake grpc protobuf
```

## Building

```bash
mkdir build
cd build
cmake ..
make -j $(nproc)
```

## Running

Start the server (with port set to 50051):
```bash
./spice_server --port=50051 --nofind_simulators --static_installs=../static_installs.pb.txt
```

## Using the example Python client

The `testdata` directory contains example netlists to run through simulators. For example, you can run `Xyce` on `cmost_inverter` by running:

```
cd testdata/cmost_inverter
/path/to/Xyce main.sp
```


Or you could have the `spice_server` run it for you by sending the test data to the server with the example python3 `client.py`. First generat the required protobufs:

```
cd example_client
./generate_protobufs.sh
```

Then:
1. changing the values in `static_installs.pb.txt` to suit your system;
1. running the server with the above comand; and
1. running the client on the testdata dir:

```
example_client/client.py testdata/cmos_inverter testdata/cmos_inverter/main.sp
```

## Querying

```bash
~/go/bin/grpcurl -plaintext localhost:50051 list
```

## netlisters

spice_server uses scripts in VLSIR's VlsirTools to convert VLSIR netlist
protobufs to different netlist formats.

It does this by invoking the python scripts directly from within the C++
program. For now, the python scripts are pulled in directly from the vlsir_repo
submodule in this repo. An alternative would to make them available as
system-wide (or user-wide, or venv-wide) packages instead.
