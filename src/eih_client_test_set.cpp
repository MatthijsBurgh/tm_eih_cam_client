#include <iostream>
#include <vector>

#include "eih_camera_api_client.h"
#define TEST_SINGLE_FUNCTION

int main() {
  std::string server_address = "172.25.181.19:15567";  // TO DO :remove

  TmEIHCamera::EIHCameraApiClient client(server_address);
  TmEIHCamera::GrpcResult grpc_result;

#ifndef TEST_SINGLE_FUNCTION
  // set capturing settings
  TmEIHConfig::SetCapturingSettingsRequest capturing_settings_req;
  capturing_settings_req.shutter_time = 36088;
  capturing_settings_req.gain = 3;
  capturing_settings_req.wb_redratio = 61;
  capturing_settings_req.wb_greenratio = 1;
  capturing_settings_req.wb_blueratio = 50;
  capturing_settings_req.focus = 6;
  capturing_settings_req.image_size = "1M";  // 1M = 1280*960, 5M = 2592*1944
  client.setCapturingSettings(grpc_result, capturing_settings_req);

  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "-------------------" << std::endl;

#else
  // Single-function
  // set shutter time
  int shutter_time = 36088;
  client.setShutterTime(grpc_result, shutter_time);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "-------------------" << std::endl;

  // set gain
  int gain = 4;
  client.setGain(grpc_result, gain);
  std::cout << "grpc status: " << grpc_result.status << std::endl;
  std::cout << "grpc error_message: " << grpc_result.error_message << std::endl;
  std::cout << "-------------------" << std::endl;

#endif

  return 0;
}