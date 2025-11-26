#!/bin/bash
/usr/bin/env python3 -m grpc_tools.protoc \
  --proto_path ../vlsir \
  --proto_path ../proto \
  ../vlsir/*.proto \
  ../vlsir/*/*.proto \
  ../proto/*.proto \
  --grpc_python_out=. \
  --python_out=.
