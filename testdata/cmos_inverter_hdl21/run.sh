#!/bin/bash

# Need Xyce in the $PATH.
export PATH="${HOME}/XyceInstall/Serial/bin:${PATH}"

# Need Hdl21, Hdl21's Sky130 package, the VLSIR python bindings and VlisrTools
# all in the Python module search path:
export PYTHONPATH="${HOME}/src/Hdl21:${HOME}/src/spice_server/vlsir_repo/VlsirTools:${HOME}/src/spice_server/vlsir_repo/bindings/python:${HOME}/src/Hdl21/pdks/Sky130/"

./inverter.py
