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

  ImageConfiguration image_config;
  client.getImageConfiguration(result, image_config);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "ImageType: " << image_config.ImageType << std::endl;
  std::cout << "ImageSize: " << image_config.ImageSize << std::endl;
  std::cout << "ImageWidth: " << image_config.ImageWidth << std::endl;
  std::cout << "ImageHeight: " << image_config.ImageHeight << std::endl;
  std::cout << "PixelFormat: " << image_config.PixelFormat << std::endl;

  std::vector<unsigned char> byte_data;
  while (true) {
    client.getImageData(result, byte_data);
    if (!byte_data.empty()) {
      cv::Mat image = cv::imdecode(cv::Mat(byte_data), cv::IMREAD_COLOR);
      cv::resize(image, image, cv::Size(480, 360));
      cv::imshow("Received Image", image);
      cv::waitKey(1);
    }
  }

  return 0;
}