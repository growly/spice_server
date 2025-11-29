#!/usr/bin/env python3

import os
from hdl21.prefix import m, u, n, p, f
import hdl21 as h
import hdl21.primitives as primitives
import hdl21.sim as sim
import sky130_hdl21 as sky130
import vlsir.circuit_pb2 as vlsir_circuit
import vlsirtools.spice as spice
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


PDK_PATH = Path(os.environ.get("PDK_ROOT")) / "sky130A"
#PDK_PATH = "/home/arya/src/pdk-root/sky130A_dan"
sky130.install = sky130.Install(
    pdk_path=PDK_PATH,
    lib_path="libs.tech/ngspice/sky130.lib.spice",
    model_ref=""    # NOTE(aryap): This seems unused.
)


def scale_params_for_sky130(module: h.Module):
    SCALE = 1E6
    for name, instance in mux.instances.items():
        for param, value in instance.of.params.items():
            new_value = (value * SCALE).scale(m)
            instance.of.params[param] = new_value


HIGH = 1.8  # volts
LOW = 0.0   # volts


@h.module
class Inverter:

    in_signal, out_signal, power, ground = h.Ports(4)
    pmos = sky130.primitives.PMOS_1p8V_STD()(d=out_signal,
                                             g=in_signal,
                                             s=power,
                                             b=power)
    nmos = sky130.primitives.NMOS_1p8V_STD()(d=out_signal,
                                             g=in_signal,
                                             s=ground,
                                             b=ground)


@sim.sim
class Sky130MuxSim:

    @h.module
    class Tb:   # required to be named 'Tb'?
        # Seems to be a requirement of a 'Tb' to have a "single, scalar port".
        VSS = h.Port()

        VDD, out_signal = h.Signals(2)

        dut = Inverter()(ground=VSS)

        power = primitives.DcVoltageSource(dc=HIGH)(p=dut.power, n=VSS)

        load = primitives.IdealCapacitor(c=2*f)(p=dut.out_signal, n=VSS)

        input_driver = primitives.PulseVoltageSource(
            v1=LOW,
            delay=50*p,
            v2=HIGH,
            rise=50*p,
            fall=50*p,
            period=1*n,
            width=500*p)(p=dut.in_signal, n=VSS)

    includes = sky130.install.include(h.pdk.Corner.TYP)
    tran = sim.Tran(tstop=4*n, tstep=1*p)


def plot(tran_result: spice.sim_data.TranResult):
    data = tran_result.data

    lines = [
        "V(XTOP:DUT_IN_SIGNAL)",
        "V(XTOP:DUT_OUT_SIGNAL)",
    ]

    fig, subs = plt.subplots(len(lines))

    for i, line in enumerate(lines):
        sub = subs[i]
        sub.plot(data["TIME"], data[line], label=line)
        sub.set_title(line)
        #sub.set_ylabel('V')
        sub.legend()
        sub.set_ylim(-0.1, 2.0)
        sub.set_yticks(np.arange(-0.1, 2.0, step=0.4))
        sub.tick_params(axis='y', labelsize=4)
        sub.tick_params(axis='x', labelsize=4)

    plt.ylabel('V')
    plt.xlabel('Time (s)')
    plt.savefig('plot.png', dpi=300)
        

def main():
    options = spice.SimOptions(
        simulator=spice.SupportedSimulators.XYCE,
        fmt=spice.ResultFormat.SIM_DATA,
        rundir="./scratch"
    )
    #if not spice.xyce.available():
    #    print("spice is not available!")
    #    return

    #scale_params(mux)

    results = Sky130MuxSim.run(options)

    tran_results = None
    for analysis in results.an:
        if isinstance(analysis, spice.sim_data.TranResult):
            tran_results = analysis

    if tran_results:
        print(tran_results)
        plot(tran_results)


if __name__ == "__main__":
    main()
