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
make
```

## Running

Start the server (default port 50051):
```bash
./spice_server
```

Or specify a custom address:
```bash
./spice_server 0.0.0.0:8080
```

## Querying

```bash
~/go/bin/grpcurl -plaintext localhost:50051 list
```

In its initial form you can execute any command and that is bad:
```bash
~/go/bin/grpcurl -plaintext -d @     localhost:50051 spicetrader.SpiceSimulator/RunSimulation << EOM
{
  "netlist": "",
  "simulator": "cat",
  "args": ["/dev/random"]
}
EOM
```
