FROM debian:13.2

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
  python3-pip
EOF

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

# Build and install spice_server.
ADD https://github.com/growly/spice_server /pkg/



# Configure installed simulators.


# Run.
