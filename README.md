# SpiceServer

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

(UNTESTED) On macOS with Homebrew:
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

You can provide override paths to the Python modules for `vlsirtools` and the
Python VLSIR bindings with the `--python_vlsir` and `--python_vlsirtools` flags
(see `src/netlister.cc` or `--help`).


## Using the example Python client to submit VLSIR netlists

The `testdata/cmos_inverter_hdl21` directory contains an example Hdl21
description of a basic CMOS inverter for the Sky130 PDK *and* a simple
testbench to demonstrate it. Hdl21 can output a `vlsir::spice::SimInput`
protobuf containing the testbench and design under test, which the example
client can submit to the spice server:

```bash
cd example_client
PYTHONPATH=${HOME}/src/Hdl21::${HOME}/src/Hdl21/pdks/Sky130 ./client.py
```

## Using the example Python client to submit Spice files

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

## TODO

I'm putting this here because I am on a plane and I can't be opening issues without paying United more money.

1. Need to tidy up how subprocesses are spawned: we're limited by dependencies on `numpy` to one interpreter session for all runs (it doesn't seem to like its DLLs being unloaded/loaded again). Seems safer to just fork and run python directly instead. (This is very silly.)
1. With (pending) changes to VlsirTools netlisters, Xyce, Spectre spice decks are handled. What about ngspice, others?
1. There are gaps in which analysis methods are supported by which simulators. This is probably documents for Hdl21/VLSIR and applies here too.
1. Results need to be parsed and returned in a SimResult. This will probably have to be implemented per simulator too. Xyce can write CSV and has a separate output file for all `.measure` statements. `Hdl21` implements some version of this already.
1. It would be nice if the example client could handle more Hdl21 modules than just the single baked-in example.
