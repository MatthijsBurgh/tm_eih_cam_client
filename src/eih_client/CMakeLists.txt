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
  
# eihcam_client
add_library(eihcam_client
  src/EIHCameraApiClient.cpp
  )

target_link_libraries(eihcam_client
  eihcam_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  )

target_include_directories(eihcam_client PUBLIC include)


add_executable(${PROJECT_NAME} 
  src/EIHClientNode.cpp
  )

target_link_libraries(${PROJECT_NAME}
  eihcam_client)

ament_target_dependencies(${PROJECT_NAME} rclcpp sensor_msgs cv_bridge OpenCV)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

target_link_libraries(${PROJECT_NAME}
    eihcam_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    ${OpenCV_LIBS}
)

install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME})

ament_package()