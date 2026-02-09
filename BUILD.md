# Build Instructions

This package uses a hybrid `ament_cmake_python` build system that:
1. Generates Python protobuf/gRPC files from `.proto` definitions during build
2. Installs Python modules and scripts
3. Uses the latest gRPC (1.60+) and Protobuf (4.21+)

## Prerequisites

### System Requirements
- ROS 2 Humble or newer
- Python 3.8 or newer
- CMake 3.16 or newer

### Python Dependencies

Install the required Python packages:

```bash
pip3 install grpcio grpcio-tools protobuf opencv-python numpy
```

Or use the provided requirements file:

```bash
pip3 install -r requirements.txt
```

## Building

### Standard ROS 2 Build

```bash
# Create workspace
mkdir -p ~/tm_eih_ws/src
cd ~/tm_eih_ws/src

# Clone repository
git clone <repository-url> tm_eih_cam_client

# Install dependencies
cd ~/tm_eih_ws
rosdep install --from-paths src --ignore-src -r -y

# Build
colcon build --packages-select tm_eih_cam_client

# Source the workspace
source install/setup.bash
```

### What Happens During Build

1. CMake generates Python protobuf files from `.proto` definitions:
   - `EIHCamera_pb2.py` - Camera data structures
   - `EIHCameraAPI_pb2.py` - API request/response messages
   - `EIHCameraAPI_pb2_grpc.py` - gRPC client stubs
   - Type stubs (`.pyi` files) for better IDE support

2. Import statements are automatically fixed to use relative imports

3. Python package and scripts are installed to the workspace

## Protobuf Generation

The protobuf files are generated automatically during the build process. If you need to regenerate them manually:

```bash
cd tm_eih_cam_client

# Generate protobuf files
python3 -m grpc_tools.protoc \
  --proto_path=proto \
  --python_out=tm_eih_cam_client \
  --grpc_python_out=tm_eih_cam_client \
  --pyi_out=tm_eih_cam_client \
  proto/EIHCamera.proto \
  proto/EIHCameraAPI.proto

# Fix imports (required for proper module imports)
python3 scripts/fix_proto_imports.py \
  tm_eih_cam_client/EIHCameraAPI_pb2.py \
  tm_eih_cam_client/EIHCameraAPI_pb2_grpc.py
```

## Verifying the Build

After building, you can verify the installation:

```bash
# Check if package is installed
ros2 pkg list | grep tm_eih_cam_client

# List executables
ros2 pkg executables tm_eih_cam_client

# Should show:
# tm_eih_cam_client eih_client_pub
# tm_eih_cam_client eih_client_test_get
# tm_eih_cam_client eih_client_test_set
# tm_eih_cam_client image_sub
```

## Troubleshooting

### Import Errors

If you encounter import errors with the protobuf files, ensure:
1. The protobuf files are generated in `tm_eih_cam_client/` directory
2. The import fix script has been run
3. The package is properly installed

### Missing Dependencies

If you get errors about missing Python modules:
```bash
pip3 install grpcio grpcio-tools protobuf
```

### Build Fails

If the build fails, check:
1. Python 3 is available and in PATH
2. `grpc_tools.protoc` module is installed: `python3 -m grpc_tools.protoc --version`
3. CMake can find Python: `cmake --find-package -DNAME=Python3 -DCOMPILER_ID=GNU -DLANGUAGE=C -DMODE=EXIST`
