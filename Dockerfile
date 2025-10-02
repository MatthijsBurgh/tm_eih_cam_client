FROM ros:humble

RUN apt-get update && apt-get install -y \
  build-essential autoconf libtool pkg-config \
  git automake curl cmake make g++ unzip libsystemd-dev \
  zlib1g-dev libssl-dev ca-certificates \
  ros-humble-vision-opencv \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

## install protobuf first, then grpc
ENV GRPC_RELEASE_TAG=v1.72.0
RUN git clone --recurse-submodules --depth 1 --shallow-submodules -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc /var/local/git/grpc && \
  cd /var/local/git/grpc && \
  mkdir -p cmake/build && \
  cd cmake/build && \
  cmake -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF \
  -DCMAKE_CXX_STANDARD=17 \
  ../.. && \
  # make -j$(nproc) && make install && make clean && ldconfig
  make -j4 && make install && make clean && ldconfig

WORKDIR /workspaces
