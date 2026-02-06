cmake_minimum_required(VERSION 3.16)

project(tm_eih_cam_client)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# Find Python3
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Generate Python protobuf files
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/tm_eih_cam_client")

# Create output directory
file(MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})

# Proto files
set(PROTO_FILES
  "${PROTO_DIR}/EIHCamera.proto"
  "${PROTO_DIR}/EIHCameraAPI.proto"
)

# Generated Python files
set(GENERATED_PY_FILES
  "${PROTO_OUTPUT_DIR}/EIHCamera_pb2.py"
  "${PROTO_OUTPUT_DIR}/EIHCamera_pb2.pyi"
  "${PROTO_OUTPUT_DIR}/EIHCameraAPI_pb2.py"
  "${PROTO_OUTPUT_DIR}/EIHCameraAPI_pb2.pyi"
  "${PROTO_OUTPUT_DIR}/EIHCameraAPI_pb2_grpc.py"
)

# Custom command to generate Python protobuf files
add_custom_command(
  OUTPUT ${GENERATED_PY_FILES}
  COMMAND ${Python3_EXECUTABLE} -m grpc_tools.protoc
    --proto_path=${PROTO_DIR}
    --python_out=${PROTO_OUTPUT_DIR}
    --grpc_python_out=${PROTO_OUTPUT_DIR}
    --pyi_out=${PROTO_OUTPUT_DIR}
    ${PROTO_FILES}
  # Fix imports to use relative imports
  COMMAND ${Python3_EXECUTABLE} -c "import re; import sys; [__import__('pathlib').Path(f).write_text(re.sub(r'^import (EIHCamera(?:API)?_pb2)', r'from . import \\\\1', __import__('pathlib').Path(f).read_text(), flags=re.MULTILINE)) for f in ['${PROTO_OUTPUT_DIR}/EIHCameraAPI_pb2.py', '${PROTO_OUTPUT_DIR}/EIHCameraAPI_pb2_grpc.py']]"
  DEPENDS ${PROTO_FILES}
  COMMENT "Generating and fixing Python protobuf files"
)

# Custom target to ensure proto files are generated
add_custom_target(generate_proto_py ALL
  DEPENDS ${GENERATED_PY_FILES}
)

# Install Python package
ament_python_install_package(${PROJECT_NAME}
  PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
)

# Install generated proto files to the package
install(
  FILES ${GENERATED_PY_FILES}
  DESTINATION "${PYTHON_INSTALL_DIR}/${PROJECT_NAME}"
)

# Install Python scripts
install(PROGRAMS
  ${PROJECT_NAME}/eih_client_pub_node.py
  ${PROJECT_NAME}/image_sub_node.py
  ${PROJECT_NAME}/eih_client_test_get.py
  ${PROJECT_NAME}/eih_client_test_set.py
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch and config files
install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
