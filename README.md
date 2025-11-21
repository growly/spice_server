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

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── proto/                  # Protocol buffer definitions
│   └── spice_simulator.proto
├── include/                # Header files
│   └── simulator_service.h
├── src/                    # Source files
│   ├── main.cpp
│   └── simulator_service.cpp
└── README.md
```

## gRPC Service

The service provides one RPC method:

- `RunSimulation`: Accepts a simulation request and streams output back to the client

### Request Format

```protobuf
message SimulationRequest {
  string netlist = 1;           // SPICE netlist content
  string simulator = 2;         // Simulator name (e.g., "ngspice")
  repeated string args = 3;     // Additional arguments
}
```

### Response Format

```protobuf
message SimulationResponse {
  string output = 1;            // Output line
  StreamType stream_type = 2;   // STDOUT or STDERR
  optional int32 exit_code = 3; // Exit code (final message only)
  bool done = 4;                // Whether this is the final message
}
```

## Example Usage

The service expects the simulator to be available in the system PATH. Make sure you have installed the SPICE simulator you want to use (e.g., ngspice).

## License

BSD
