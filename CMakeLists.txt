cmake_minimum_required(VERSION 3.16)

project(eih_client)

include(common.cmake)

# Proto files
get_filename_component(eihcam_api_proto "proto/EIHCameraAPI.proto" ABSOLUTE)
get_filename_component(eihcam_api_proto_path "${eihcam_api_proto}" PATH)
get_filename_component(eihcam_proto "proto/EIHCamera.proto" ABSOLUTE)
get_filename_component(eihcam_proto_path "${eihcam_proto}" PATH)

# Generated sources for EIHCameraAPI.proto
set(eihcam_api_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/EIHCameraAPI.pb.cc")
set(eihcam_api_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/EIHCameraAPI.pb.h")
set(eihcam_api_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/EIHCameraAPI.grpc.pb.cc")
set(eihcam_api_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/EIHCameraAPI.grpc.pb.h")
add_custom_command(
      OUTPUT "${eihcam_api_proto_srcs}" "${eihcam_api_proto_hdrs}" "${eihcam_api_grpc_srcs}" "${eihcam_api_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${eihcam_api_proto_path}"
	--proto_path=/include/:.
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${eihcam_api_proto}"
      DEPENDS "${eihcam_api_proto}")

# Generated sources for EIHCamera.proto
set(eihcam_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/EIHCamera.pb.cc")
set(eihcam_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/EIHCamera.pb.h")
set(eihcam_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/EIHCamera.grpc.pb.cc")
set(eihcam_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/EIHCamera.grpc.pb.h")
add_custom_command(
      OUTPUT "${eihcam_proto_srcs}" "${eihcam_proto_hdrs}" "${eihcam_grpc_srcs}" "${eihcam_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${eihcam_proto_path}"
	--proto_path=/include/:.
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${eihcam_proto}"
      DEPENDS "${eihcam_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# eihcam_grpc_proto
add_library(eihcam_grpc_proto
  ${eihcam_api_grpc_srcs}
  ${eihcam_api_grpc_hdrs}
  ${eihcam_api_proto_srcs}
  ${eihcam_api_proto_hdrs}
  ${eihcam_grpc_srcs}
  ${eihcam_grpc_hdrs}
  ${eihcam_proto_srcs}
  ${eihcam_proto_hdrs})

target_link_libraries(eihcam_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
  
# eihcam_api_client
add_library(eihcam_api_client
  src/eih_camera_api_client.cpp
  )

target_link_libraries(eihcam_api_client
  eihcam_grpc_proto
  )

target_include_directories(eihcam_api_client PUBLIC include)

# eih client node
add_executable(eih_client_pub
  src/eih_client_pub_node.cpp
  )

target_link_libraries(eih_client_pub
  eihcam_api_client)

ament_target_dependencies(eih_client_pub rclcpp sensor_msgs cv_bridge OpenCV)

# image sub node
add_executable(image_sub 
  src/image_sub_node.cpp
  )

ament_target_dependencies(image_sub rclcpp sensor_msgs cv_bridge OpenCV)

install(TARGETS eih_client_pub image_sub
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_package()
