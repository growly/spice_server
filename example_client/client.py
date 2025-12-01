#!/usr/bin/env python3

import os
import grpc
import spice_simulator_pb2
import spice_simulator_pb2_grpc
import sys
import pathlib
import tabulate

# We do this to avoid having to install anything, so that the script works for
# its intended (debugging, development) purposes. The only additional
# dependencies are Hdl21 and its Sky130 PDK modules, which you should make
# available somewhere on disk (such as by installing the package).
import sys
sys.path.extend([
    '../testdata',
    '../vlsir_repo/VlsirTools',
    '../vlsir_repo/bindings/python'
])

# The default VLSIR message is this example Hdl21 module.
import cmos_inverter_hdl21.inverter as example_cmos_inverter

def usage():
    print('''Usage:
./client.py <test directory> <entry spice netlist>

With no options, client will submit a VLSIR test message instead.

e.g. (from this directory)
./client.py ../testdata/cmos_inverter ../testdata/cmos_inverter/main.sp''')

def bundle_up_files(target_dir, first_file, request_out):
    root = os.path.realpath(target_dir)
    first_file = os.path.realpath(first_file)
    if not first_file.startswith(root):
        print('error: the first file must a be child of the test directory')
        sys.exit(1)

    loaded_files = []
    print('scanning', root)
    rows = [['', 'bytes', 'path']]
    for path, _, files in os.walk(root):
        relative_dir = os.path.relpath(path, root)
        for file in files:
            row = ['']
            full_path = os.path.realpath(os.path.join(root, relative_dir, file))
            relative_path = os.path.join(relative_dir, file)
            file_info_pb = spice_simulator_pb2.FileInfo()
            file_info_pb.path = relative_path
            with open(full_path, 'rb') as f:
                #print(f'reading {full_path}')
                file_info_pb.data = f.read()
                row.append(len(file_info_pb.data))
                row.append(full_path)

            if full_path == first_file:
                # Put this file first
                loaded_files.insert(0, file_info_pb)
                row[0] = '*'
            else:
                loaded_files.append(file_info_pb)
            rows.append(row)

    request_out.verbatim_files.files.extend(loaded_files)
    print(tabulate.tabulate(rows, headers='firstrow'))

def main():
    # TODO(aryap): There are better ways to do this, like with using argparse
    # or optparse.
    use_vlsir = True
    if len(sys.argv) == 3:
        target_dir = sys.argv[1]
        main_file = sys.argv[2]
        use_vlsir = False
    elif len(sys.argv) != 1:
        usage()
        sys.exit(1)

    request = spice_simulator_pb2.SimulationRequest()
    request.simulator = spice_simulator_pb2.Flavour.XYCE

    if use_vlsir:
        request.vlsir_sim_input.CopyFrom(
                example_cmos_inverter.to_sim_input_pb())
    else:
        bundle_up_files(target_dir, main_file, request)

    with grpc.insecure_channel("localhost:50051") as channel:
        stub = spice_simulator_pb2_grpc.SpiceSimulatorStub(channel)
        for response in stub.RunSimulation(request):
            for line in response.output.splitlines():
                print('remote:', line)

if __name__ == '__main__':
    main()
