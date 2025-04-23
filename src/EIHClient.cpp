#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "EIHCameraApiClient.h"
using namespace TmEIHCamera;

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);
  GrpcResult result;

  bool is_connected = false;
  client.isCameraConnected(result, is_connected);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "isCameraConnected: " << is_connected << std::endl;

  TmEIHConfig::Image eih_image;
  // TmEIHConfig::Image::Configuration image_config;
  client.getImageConfiguration(result, eih_image.config);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "ImageType: " << eih_image.config.ImageType << std::endl;
  std::cout << "ImageSize: " << eih_image.config.ImageSize << std::endl;
  std::cout << "ImageWidth: " << eih_image.config.ImageWidth << std::endl;
  std::cout << "ImageHeight: " << eih_image.config.ImageHeight << std::endl;
  std::cout << "PixelFormat: " << eih_image.config.PixelFormat << std::endl;

  /*TmEIHConfig::HandEyeArray hand_eye_array;
  client.getHandEyeParameters(result, hand_eye_array);
  // to do (struct)
  std::cout << "  handeye_x: " << hand_eye_array.handeye_x << std::endl;
  std::cout << "  handeye_y: " << hand_eye_array.handeye_y << std::endl;
  std::cout << "  handeye_z: " << hand_eye_array.handeye_z << std::endl;
  std::cout << "  handeye_rx: " << hand_eye_array.handeye_rx << std::endl;
  std::cout << "  handeye_ry: " << hand_eye_array.handeye_ry << std::endl;
  std::cout << "  handeye_rz: " << hand_eye_array.handeye_rz << std::endl;
*/
  // std::vector<unsigned char> byte_data;
  while (true) {
    client.getImageData(result, eih_image.byte_data);
    if (!eih_image.byte_data.empty()) {
      cv::Mat image =
          cv::imdecode(cv::Mat(eih_image.byte_data), cv::IMREAD_COLOR);
      cv::resize(image, image, cv::Size(480, 360));
      cv::imshow("Received Image", image);
      cv::waitKey(1);
    }
  }

  return 0;
}