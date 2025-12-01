# spice_server client

This is an example of how to implement a gRPC client for the spice-server. It can either:

1. Bundle up all of the files in a directory and submit them to the server to run; or
2. Submit an Hdl21 testbench and any dependent modules to the server directly.

## Submitting Hdl21 modules

```bash
PYTHONPATH=${HOME}/src/Hdl21::${HOME}/src/Hdl21/pdks/Sky130 ./client.py
```

When it works you should see something like:

```
remote: 
remote: *****
remote: ***** Welcome to the Xyce(TM) Parallel Electronic Simulator
remote: *****
remote: ***** This is version Xyce Release 7.10.0-opensource
remote: ***** Date: Sun Nov 30 18:30:10 PST 2025
remote: 
remote: 
remote: ***** Executing netlist /tmp/spice_server.v2xwRZ/main.sp
remote: 
remote: ***** Reading and parsing netlist...
remote: Netlist warning in file
remote:  /home/arya/.volare/sky130A/libs.tech/ngspice/corners/../../../libs.ref/sky130_fd_pr/spice/sky130_fd_pr__pfet_01v8__tt.pm3.spice
remote:  at or near line 24776
remote:  Device model XTOP:XDUT:XPMOS:SKY130_FD_PR__PFET_01V8__MODEL.89: Model card
remote:  specifies BSIM4 version 4.5 which is older than the oldest version supported
remote:  in Xyce (4.6.1).  Using oldest version available.
remote:  
remote: Netlist warning in file
remote:  /home/arya/.volare/sky130A/libs.tech/ngspice/corners/../../../libs.ref/sky130_fd_pr/spice/sky130_fd_pr__pfet_01v8__tt.pm3.spice
remote:  at or near line 24776
remote:  Device model XTOP:XDUT:XPMOS:SKY130_FD_PR__PFET_01V8__MODEL.89: Given pbswgd
remote:  is less than 0.1. Pbswgd is set to 0.1
remote: Netlist warning in file
remote:  /home/arya/.volare/sky130A/libs.tech/ngspice/corners/../../../libs.ref/sky130_fd_pr/spice/sky130_fd_pr__nfet_01v8__tt.pm3.spice
remote:  at or near line 24151
remote:  Device model XTOP:XDUT:XNMOS:SKY130_FD_PR__NFET_01V8__MODEL.89: Model card
remote:  specifies BSIM4 version 4.5 which is older than the oldest version supported
remote:  in Xyce (4.6.1).  Using oldest version available.
remote:  
remote: Netlist warning in file
remote:  /home/arya/.volare/sky130A/libs.tech/ngspice/corners/../../../libs.ref/sky130_fd_pr/spice/sky130_fd_pr__nfet_01v8__tt.pm3.spice
remote:  at or near line 24151
remote:  Device model XTOP:XDUT:XNMOS:SKY130_FD_PR__NFET_01V8__MODEL.89: Given pbswgd
remote:  is less than 0.1. Pbswgd is set to 0.1
remote: ***** Setting up topology...
remote: 
remote: ***** Device Count Summary ...
remote:        C level 1 (Capacitor)                  1
remote:        M level 14 (BSIM4)                     2
remote:        V level 1 (Independent Voltage Source) 2
remote:        ----------------------------------------
remote:        Total Devices                          5
remote: ***** Setting up matrix structure...
remote: ***** Number of Unknowns = 15
remote: ***** Initializing...
remote: 
remote: ***** Beginning DC Operating Point Calculation...
remote: 
remote: ***** Beginning Transient Calculation...
remote: 
remote: ***** Percent complete: 1.06875 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 2.0753 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 3.25471 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 5.02827 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 6.80183 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 9.30183 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 11.8018 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 14.3018 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 15.3828 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 16.4466 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 17.5215 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 18.792 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 20.3556 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 22.701 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 25.201 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 26.25 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 27.295 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 28.3656 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 29.9135 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 31.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 33.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 36.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 38.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 40 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 41.0102 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 42.0329 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 43.4011 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 44.5738 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 46.1374 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 47.701 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 50.201 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 51.25 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 52.295 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 53.3656 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 54.9135 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 56.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 58.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 61.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 63.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 65 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 66.0102 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 67.0329 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 68.4011 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 69.5738 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 71.1374 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 72.701 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 75.201 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 76.25 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 77.295 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 78.3656 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 79.9135 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 81.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 83.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 86.4614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 88.9614 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 90 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 91.0102 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 92.0329 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 93.4011 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 94.5738 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 96.1374 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Percent complete: 97.701 %
remote: ***** Current system time: Sun Nov 30 18:30:10 2025
remote: ***** Estimated time to completion:  0 sec.
remote: 
remote: ***** Problem read in and set up time: 0.31751 seconds
remote:  ***** DCOP time: 0.000601053 seconds.  Breakdown follows:
remote: 	Number Successful Steps Taken:		1
remote: 	Number Failed Steps Attempted:		0
remote: 	Number Jacobians Evaluated:		4
remote: 	Number Linear Solves:			4
remote: 	Number Failed Linear Solves:		0
remote: 	Number Residual Evaluations:		6
remote: 	Number Nonlinear Convergence Failures:	0
remote: 	Total Residual Load Time:		2.16961e-05 seconds
remote: 	Total Jacobian Load Time:		5.96046e-06 seconds
remote: 	Total Linear Solution Time:		0.000172138 seconds
remote: 
remote:  ***** Transient Stepping time: 0.00960207 seconds.  Breakdown follows:
remote: 	Number Successful Steps Taken:		432
remote: 	Number Failed Steps Attempted:		43
remote: 	Number Jacobians Evaluated:		993
remote: 	Number Linear Solves:			993
remote: 	Number Failed Linear Solves:		0
remote: 	Number Residual Evaluations:		1468
remote: 	Number Nonlinear Convergence Failures:	0
remote: 	Total Residual Load Time:		0.00205135 seconds
remote: 	Total Jacobian Load Time:		0.000448942 seconds
remote: 	Total Linear Solution Time:		0.00159931 seconds
remote: 
remote: 
remote: ***** Solution Summary *****
remote: 	Number Successful Steps Taken:		433
remote: 	Number Failed Steps Attempted:		43
remote: 	Number Jacobians Evaluated:		997
remote: 	Number Linear Solves:			997
remote: 	Number Failed Linear Solves:		0
remote: 	Number Residual Evaluations:		1474
remote: 	Number Nonlinear Convergence Failures:	0
remote: 	Total Residual Load Time:		0.00207305 seconds
remote: 	Total Jacobian Load Time:		0.000454903 seconds
remote: 	Total Linear Solution Time:		0.00177145 seconds
remote: 
remote: 
remote: ***** Total Simulation Solvers Run Time: 0.0102479 seconds
remote: ***** Total Elapsed Run Time:            0.327755 seconds
remote: *****
remote: ***** End of Xyce(TM) Simulation
remote: *****
remote: 
remote: Timing summary of 1 processor
remote:                  Stats                   Count       CPU Time              Wall Time
remote: ---------------------------------------- ----- --------------------- ---------------------
remote: Xyce                                         1        0.690 (100.0%)        0.328 (100.0%)
remote:   Analysis                                   1        0.010 ( 1.47%)        0.010 ( 3.12%)
remote:     Transient                                1        0.010 ( 1.47%)        0.010 ( 3.12%)
remote:       Nonlinear Solve                      476        0.007 ( 1.06%)        0.007 ( 2.25%)
remote:         Residual                          1474        0.002 ( 0.36%)        0.003 ( 0.77%)
remote:         Jacobian                           997        0.001 ( 0.10%)        0.001 ( 0.22%)
remote:         Linear Solve                       997        0.002 ( 0.30%)        0.002 ( 0.63%)
remote:       Successful DCOP Steps                  1        0.000 ( 0.02%)        0.000 ( 0.05%)
remote:       Successful Step                      432        0.002 ( 0.22%)        0.002 ( 0.48%)
remote:       Failed Steps                          43        0.000 (<0.01%)        0.000 (<0.01%)
remote:   Netlist Import                             1        0.667 (96.74%)        0.313 (95.37%)
remote:     Parse Context                            1        0.658 (95.34%)        0.303 (92.32%)
remote:     Distribute Devices                       1        0.006 ( 0.84%)        0.006 ( 1.80%)
remote:     Verify Devices                           1        0.000 (<0.01%)        0.000 (<0.01%)
remote:     Instantiate                              1        0.000 (<0.01%)        0.000 ( 0.02%)
remote:   Late Initialization                        1        0.001 ( 0.16%)        0.001 ( 0.34%)
remote:     Global Indices                           1        0.000 (<0.01%)        0.000 (<0.01%)
remote:   Setup Matrix Structure                     1        0.000 ( 0.02%)        0.000 ( 0.04%)
remote: 
```
