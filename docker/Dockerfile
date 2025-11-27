FROM debian:13.2-slim

# Add debian packages.
RUN <<EOF
apt update
apt install -y \
  build-essential \
  gfortran \
  make \
  cmake \
  bison \
  flex \
  libfl-dev \
  libfftw3-dev \
  libsuitesparse-dev \
  libblas-dev \
  liblapack-dev \
  libtool \
  autoconf \
  automake \
  git \
  libopenmpi-dev \
  openmpi-bin \
  libnetcdf-dev \
  python3 \
  python3-pip \
  wget \
  libc-ares-dev \
  python3-full \
  python3-pandas
EOF

# Add development dependencies for spice_server.
RUN git clone -b v1.16.0 --depth 1 https://github.com/google/googletest.git
WORKDIR googletest/build
RUN cmake ../
RUN make -j $(nproc) && make install

WORKDIR /src
RUN git clone -b gperftools-2.16.90 --depth 1 https://github.com/gperftools/gperftools.git
WORKDIR gperftools
RUN ./autogen.sh
RUN ./configure
RUN make -j $(nproc) && make install

WORKDIR /src
RUN git clone -b v2.2.2 --depth 1 https://github.com/gflags/gflags.git
WORKDIR gflags/build
RUN cmake .. -DBUILD_SHARED_LIBS=ON
RUN make -j $(nproc) && make install

WORKDIR /src
RUN git clone -b v0.7.1 --depth 1 https://github.com/google/glog.git
WORKDIR glog
RUN cmake -S . -B build -G "Unix Makefiles"
RUN cmake --build build
RUN cmake --build build --target install

WORKDIR /src
RUN git clone https://github.com/abseil/abseil-cpp.git
WORKDIR abseil-cpp
RUN git checkout 1a31b81c0a467c1c8e229b9fc172a4eb0db5bd85
WORKDIR build
RUN cmake -DABSL_RUN_TESTS=ON -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON ../
RUN make -j $(nproc)
RUN make install

WORKDIR /src
RUN git clone -b 2024-07-02 --depth 1 https://github.com/google/re2.git
WORKDIR re2/build
RUN cmake ../
RUN make -j $(nproc)
RUN make install

WORKDIR /src
RUN wget -O - https://github.com/protocolbuffers/protobuf/releases/download/v21.5/protobuf-all-21.5.tar.gz | tar xfz -
WORKDIR protobuf-21.5
RUN ./autogen.sh
RUN ./configure
RUN make -j $(nproc)
RUN make install
# Refresh shared library cache
RUN ldconfig

WORKDIR /src
RUN wget -O - https://github.com/grpc/grpc/archive/refs/tags/v1.48.1.tar.gz | tar xfz -
WORKDIR grpc-1.48.1/cmake/build
RUN cmake \
  -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF \
  -DgRPC_CARES_PROVIDER=package \
  -DgRPC_ABSL_PROVIDER=package \
  -DgRPC_PROTOBUF_PROVIDER=package \
  -DgRPC_RE2_PROVIDER=package \
  -DgRPC_SSL_PROVIDER=package \
  -DgRPC_ZLIB_PROVIDER=package \
  ../..
RUN make -j $(nproc)
RUN make install

# Add ngspice.
RUN apt install -y ngspice

# Build and install Trilinos.
ADD https://github.com/trilinos/Trilinos/archive/refs/tags/trilinos-release-14-4-0.tar.gz /pkg/
WORKDIR /pkg/
RUN tar xf trilinos-release-14-4-0.tar.gz
RUN mkdir trilinos-build-serial
WORKDIR trilinos-build-serial
ARG SRCDIR="/pkg/Trilinos-trilinos-release-14-4-0"
ARG LIBINSTALL="/pkg/XyceLibs/Serial"
ARG FLAGS="-O3 -fPIC"
RUN <<EOF
cmake \
  -G "Unix Makefiles" \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_Fortran_COMPILER=gfortran \
  -DCMAKE_CXX_FLAGS="$FLAGS" \
  -DCMAKE_C_FLAGS="$FLAGS" \
  -DCMAKE_Fortran_FLAGS="$FLAGS" \
  -DCMAKE_INSTALL_PREFIX=${LIBINSTALL} \
  -DCMAKE_MAKE_PROGRAM="make" \
  -DTrilinos_ENABLE_NOX=ON \
  -DNOX_ENABLE_LOCA=ON \
  -DTrilinos_ENABLE_EpetraExt=ON \
  -DEpetraExt_BUILD_BTF=ON \
  -DEpetraExt_BUILD_EXPERIMENTAL=ON \
  -DEpetraExt_BUILD_GRAPH_REORDERINGS=ON \
  -DTrilinos_ENABLE_TrilinosCouplings=ON \
  -DTrilinos_ENABLE_Ifpack=ON \
  -DTrilinos_ENABLE_AztecOO=ON \
  -DTrilinos_ENABLE_Belos=ON \
  -DTrilinos_ENABLE_Teuchos=ON \
  -DTrilinos_ENABLE_COMPLEX_DOUBLE=ON \
  -DTrilinos_ENABLE_Amesos=ON \
  -DAmesos_ENABLE_KLU=ON \
  -DTrilinos_ENABLE_Amesos2=ON \
  -DAmesos2_ENABLE_KLU2=ON \
  -DAmesos2_ENABLE_Basker=ON \
  -DTrilinos_ENABLE_Sacado=ON \
  -DTrilinos_ENABLE_Stokhos=ON \
  -DTrilinos_ENABLE_Kokkos=ON \
  -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES=OFF \
  -DTrilinos_ENABLE_CXX11=ON \
  -DTPL_ENABLE_AMD=ON \
  -DAMD_LIBRARY_DIRS="/usr/lib" \
  -DTPL_AMD_INCLUDE_DIRS="/usr/include/suitesparse" \
  -DTPL_ENABLE_BLAS=ON \
  -DTPL_ENABLE_LAPACK=ON \
  $SRCDIR
EOF
RUN make -j $(nproc)
RUN make install

# 4. Build and install Xyce.
ADD https://github.com/Xyce/Xyce/archive/refs/tags/Release-7.10.0.tar.gz /pkg/Xyce-Release-7.10.0.tar.gz
WORKDIR /pkg/
RUN tar xf /pkg/Xyce-Release-7.10.0.tar.gz
RUN mkdir xyce-build-serial
WORKDIR xyce-build-serial
RUN <<EOF
cmake \
  -D CMAKE_INSTALL_PREFIX=/pkg/XyceInstall/Serial \
  -D Trilinos_ROOT=${LIBINSTALL} \
  /pkg/Xyce-Release-7.10.0
EOF
RUN cmake --build . -j $(nproc) -t install

# Add Skywater130 PDK.
ENV PDK_ROOT="/pdk/"
RUN mkdir -p ${PDK_ROOT}
WORKDIR /pkg/
RUN python3 -m venv /pkg/python3-ciel
RUN /pkg/python3-ciel/bin/python3 -m pip install --upgrade --no-cache-dir ciel
#RUN /pkg/python3-ciel/bin/ciel ls-remote --pdk-family sky130
RUN /pkg/python3-ciel/bin/ciel enable --pdk-family sky130 486fb8633c3d95ae475a5ccd68d0066c25919bb5
#RUN /pkg/python3-ciel/bin/ciel ls-remote --pdk-family gf180mcu
RUN /pkg/python3-ciel/bin/ciel enable --pdk-family gf180mcu 486fb8633c3d95ae475a5ccd68d0066c25919bb5

# Build and install spice_server.
ADD https://github.com/growly/spice_server /pkg/



# Configure installed simulators.


# Run.
