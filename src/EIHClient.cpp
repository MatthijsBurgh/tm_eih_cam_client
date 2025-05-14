#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "EIHCameraApiClient.h"
using namespace TmEIHCamera;

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);
  GrpcResult result;

#define TEST
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
  TmEIHConfig::CameraConnection cam_connect;
  client.isCameraConnected(result, cam_connect);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "EIH connection: " << cam_connect.is_connected << std::endl;
  std::cout << "EIH connection message: " << cam_connect.connection_message
            << std::endl;

  TmEIHConfig::Image eih_image;
  client.getImageConfiguration(result, eih_image.config);
  std::cout << "grpc status: " << result.status << std::endl;
  std::cout << "grpc error_message: " << result.error_message << std::endl;
  std::cout << "ImageType: " << eih_image.config.image_type << std::endl;
  std::cout << "ImageSize: " << eih_image.config.image_size << std::endl;
  std::cout << "ImageWidth: " << eih_image.config.image_width << std::endl;
  std::cout << "ImageHeight: " << eih_image.config.image_height << std::endl;
  std::cout << "PixelFormat: " << eih_image.config.pixel_format << std::endl;

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
#endif

  return 0;
}