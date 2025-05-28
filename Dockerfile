FROM ros:humble

LABEL maintainer="Steven Zheng <steven.zheng@tm-robot.com>"

RUN apt-get update && apt-get install -y \
  build-essential autoconf git pkg-config \
  automake libtool curl make g++ unzip \
  ros-humble-cv-bridge \
  && apt-get clean

# install protobuf first, then grpc
ENV GRPC_RELEASE_TAG v1.72.x
RUN git clone --recurse-submodules --depth 1 --shallow-submodules -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc /var/local/git/grpc && \
		cd /var/local/git/grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../.. && \
    make -j$(nproc) && make install && make clean && ldconfig

WORKDIR /workspace
