#include <grpcpp/grpcpp.h>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "EIHCameraApiClient.hpp"
using namespace TmEIHCamera;

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);

  bool isConnected = client.isCameraConnected();

  auto [format, encoding, width, height, additional_info] =
      client.getImageConfiguration();
  std::cout << "Image Configuration:" << std::endl;
  std::cout << "Format: " << format << std::endl;
  std::cout << "Encoding: " << encoding << std::endl;
  std::cout << "Width: " << width << std::endl;
  std::cout << "Height: " << height << std::endl;
  std::cout << "Additional Info: " << additional_info << std::endl;

  while (true) {
    std::vector<unsigned char> byteData = client.getImageData();
    if (!byteData.empty()) {
      cv::Mat image = cv::imdecode(cv::Mat(byteData), cv::IMREAD_COLOR);
      cv::resize(image, image, cv::Size(480, 360));
      cv::imshow("Received Image", image);
      cv::waitKey(1);
    }
  }

  return 0;
}