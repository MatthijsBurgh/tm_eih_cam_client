#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

#include "eih_camera_api_client.h"
using namespace TmEIHCamera;
// #define TEST_IMAGE_DATA

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);
  GrpcResult grpc_result;

  TmEIHConfig::IsCameraConnectedResponse eih_connect;
  client.isCameraConnected(grpc_result, eih_connect);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "EIH connection: " << eih_connect.is_camera_connected
            << std::endl;
  std::cout << "EIH connection message: " << eih_connect.connection_message
            << std::endl;

  TmEIHConfig::Image eih_img;
  client.getImageConfiguration(grpc_result, eih_img.config);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "ImageType: " << eih_img.config.image_type << std::endl;
  std::cout << "ImageSize: " << eih_img.config.image_size << std::endl;
  std::cout << "ImageWidth: " << eih_img.config.image_width << std::endl;
  std::cout << "ImageHeight: " << eih_img.config.image_height << std::endl;
  std::cout << "PixelFormat: " << eih_img.config.pixel_format << std::endl;

  std::vector<TmEIHConfig::Intrinsics> intrinsics_res;
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

  TmEIHConfig::HandEyeArray hand_eye_array;
  client.getHandEyeParameters(grpc_result, hand_eye_array);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "  handeye_x: " << hand_eye_array.handeye_x << std::endl;
  std::cout << "  handeye_y: " << hand_eye_array.handeye_y << std::endl;
  std::cout << "  handeye_z: " << hand_eye_array.handeye_z << std::endl;
  std::cout << "  handeye_rx: " << hand_eye_array.handeye_rx << std::endl;
  std::cout << "  handeye_ry: " << hand_eye_array.handeye_ry << std::endl;
  std::cout << "  handeye_rz: " << hand_eye_array.handeye_rz << std::endl;

  TmEIHConfig::CapturingSettings capturing_settings;
  client.getCapturingSettings(grpc_result, capturing_settings);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "ShutterTime: " << capturing_settings.shutter_time.current_value
            << " (min: " << capturing_settings.shutter_time.min_value
            << ", max: " << capturing_settings.shutter_time.max_value << ")"
            << std::endl;
  std::cout << "Gain: " << capturing_settings.gain.current_value
            << " (min: " << capturing_settings.gain.min_value
            << ", max: " << capturing_settings.gain.max_value << ")"
            << std::endl;
  std::cout << "WhiteBalance: " << std::endl;
  std::cout << "  RedRatio: "
            << capturing_settings.white_balance.red_ratio.current_value
            << " (min: " << capturing_settings.white_balance.red_ratio.min_value
            << ", max: " << capturing_settings.white_balance.red_ratio.max_value
            << ")" << std::endl;
  std::cout << "  GreenRatio: "
            << capturing_settings.white_balance.green_ratio.current_value
            << " (min: "
            << capturing_settings.white_balance.green_ratio.min_value
            << ", max: "
            << capturing_settings.white_balance.green_ratio.max_value << ")"
            << std::endl;
  std::cout << "  BlueRatio: "
            << capturing_settings.white_balance.blue_ratio.current_value
            << " (min: "
            << capturing_settings.white_balance.blue_ratio.min_value
            << ", max: "
            << capturing_settings.white_balance.blue_ratio.max_value << ")"
            << std::endl;
  std::cout << "Focus: " << capturing_settings.focus.current_value
            << " (min: " << capturing_settings.focus.min_value
            << ", max: " << capturing_settings.focus.max_value << ")"
            << std::endl;
  std::cout << "ImageSize: " << capturing_settings.image_size << std::endl;

#ifdef TEST_IMAGE_DATA
  // std::vector<unsigned char> byte_data;
  // TmEIHConfig::Image::Data eih_img;
  while (true) {
    client.getImageData(grpc_result, eih_img.data);
    if (!eih_img.data.encode_string.empty()) {
      cv::Mat image =
          cv::imdecode(cv::Mat(eih_img.data.encode_string), cv::IMREAD_COLOR);
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