# Docker image

It seems like it would be nice to have a pre-built image with all of the things we want in order to run `spice_server`:

1. OS package dependencies, a compiler, toolchains, etc.
1. Xyce, its dependency Trilinos, which is itself a pile of other packages
1. ngspice
1. All of the `spice_server` dependencies themselves, like `glog`, `gflags`, Abseil, etc.
1. Whatever PDKs we have appropriate licenses for.
1. Automatic configuration of the `spice_server`, its knowledge of installed simulators, etc.

```
docker build -t spice_server .
```
