#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

#include "eih_camera_api_client.h"
using namespace TmEIHCamera;

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);
  GrpcResult grpc_result;

// #define TEST
#ifdef TEST
  TmEIHConfig::ConfigManager manager;

  client.isCameraConnected(result, manager.cam_connect);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "EIH connection: " << manager.cam_connect.is_connected
            << std::endl;
  std::cout << "EIH connection message: "
            << manager.cam_connect.connection_message << std::endl;

  client.getImageConfiguration(result, manager.image.config);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "ImageType: " << manager.image.config.image_type << std::endl;
  std::cout << "ImageSize: " << manager.image.config.image_size << std::endl;
  std::cout << "ImageWidth: " << manager.image.config.image_width << std::endl;
  std::cout << "ImageHeight: " << manager.image.config.image_height
            << std::endl;
  std::cout << "PixelFormat: " << manager.image.config.pixel_format
            << std::endl;

  while (true) {
    client.getImageData(result, manager.image.byte_data);
    if (!manager.image.byte_data.empty()) {
      cv::Mat image =
          cv::imdecode(cv::Mat(manager.image.byte_data), cv::IMREAD_COLOR);
      cv::resize(image, image, cv::Size(480, 360));
      cv::imshow("Received Image", image);
      cv::waitKey(1);
    }
  }

#else
  TmEIHConfig::IsCameraConnectedResponse cam_connect;
  client.isCameraConnected(grpc_result, cam_connect);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "EIH connection: " << cam_connect.is_camera_connected
            << std::endl;
  std::cout << "EIH connection message: " << cam_connect.connection_message
            << std::endl;

  TmEIHConfig::Camera eih_cam;
  client.getImageConfiguration(grpc_result, eih_cam.image.config);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "ImageType: " << eih_cam.image.config.image_type << std::endl;
  std::cout << "ImageSize: " << eih_cam.image.config.image_size << std::endl;
  std::cout << "ImageWidth: " << eih_cam.image.config.image_width << std::endl;
  std::cout << "ImageHeight: " << eih_cam.image.config.image_height
            << std::endl;
  std::cout << "PixelFormat: " << eih_cam.image.config.pixel_format
            << std::endl;

  std::vector<TmEIHConfig::Camera::Intrinsics> intrinsics_res;
  client.getIntrinsics(grpc_result, intrinsics_res);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;

  for (size_t i = 0; i < intrinsics_res.size(); ++i) {
    const auto& ci = intrinsics_res[i];
    std::cout << "=== Camera " << i << " Intrinsics ===" << std::endl;
    std::cout << "FocusValue: " << ci.focus_value << std::endl;
    std::cout << "ImageWidth: " << ci.image_width << std::endl;
    std::cout << "ImageHeight: " << ci.image_height << std::endl;
    std::cout << "CameraMatrix: " << std::endl;
    std::cout << "  matrix_00: " << ci.camera_matrix.matrix_00 << std::endl;
    std::cout << "  matrix_01: " << ci.camera_matrix.matrix_01 << std::endl;
    std::cout << "  matrix_02: " << ci.camera_matrix.matrix_02 << std::endl;
    std::cout << "  matrix_10: " << ci.camera_matrix.matrix_10 << std::endl;
    std::cout << "  matrix_11: " << ci.camera_matrix.matrix_11 << std::endl;
    std::cout << "  matrix_12: " << ci.camera_matrix.matrix_12 << std::endl;
    std::cout << "  matrix_20: " << ci.camera_matrix.matrix_20 << std::endl;
    std::cout << "  matrix_21: " << ci.camera_matrix.matrix_21 << std::endl;
    std::cout << "  matrix_22: " << ci.camera_matrix.matrix_22 << std::endl;
    std::cout << "DistortionCoefficients: " << std::endl;
    std::cout << "  coefficient_00: "
              << ci.distortion_coefficients.coefficient_00 << std::endl;
    std::cout << "  coefficient_10: "
              << ci.distortion_coefficients.coefficient_10 << std::endl;
    std::cout << "  coefficient_20: "
              << ci.distortion_coefficients.coefficient_20 << std::endl;
    std::cout << "  coefficient_30: "
              << ci.distortion_coefficients.coefficient_30 << std::endl;
    std::cout << "  coefficient_40: "
              << ci.distortion_coefficients.coefficient_40 << std::endl;
  }

  // to do (struct)

  // TmEIHConfig::HandEyeArray hand_eye_array;
  // client.getHandEyeParameters(grpc_result, hand_eye_array);
  // // to do (struct)
  // std::cout << "  handeye_x: " << hand_eye_array.handeye_x << std::endl;
  // std::cout << "  handeye_y: " << hand_eye_array.handeye_y << std::endl;
  // std::cout << "  handeye_z: " << hand_eye_array.handeye_z << std::endl;
  // std::cout << "  handeye_rx: " << hand_eye_array.handeye_rx <<
  // std::endl; std::cout << "  handeye_ry: " << hand_eye_array.handeye_ry
  // << std::endl; std::cout << "  handeye_rz: " <<
  // hand_eye_array.handeye_rz << std::endl;

  // std::vector<unsigned char> byte_data;
  while (true) {
    client.getImageData(grpc_result, eih_cam.image.data);
    if (!eih_cam.image.data.encode_string.empty()) {
      cv::Mat image = cv::imdecode(cv::Mat(eih_cam.image.data.encode_string),
                                   cv::IMREAD_COLOR);
      cv::resize(image, image, cv::Size(480, 360));
      cv::imshow("Received Image", image);
      cv::waitKey(1);
    } else {
      std::cout << "Image byte data empty!" << std::endl;
      std::this_thread::sleep_for(
          std::chrono::milliseconds(500));  // delay 500ms
      // cv::waitKey(500);
    }
  }
#endif

  return 0;
}