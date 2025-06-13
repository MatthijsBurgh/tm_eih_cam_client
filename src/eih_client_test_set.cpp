#include <iostream>
#include <vector>

#include "eih_camera_api_client.h"

int main() {
  std::string server_address = "172.25.181.19:15567";  // TO DO :remove

  TmEIHCamera::EIHCameraApiClient client(server_address);
  TmEIHCamera::GrpcResult grpc_result;

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

  return 0;
}