#pragma once
#ifndef CAMERA_API_CLIENT_H
#define CAMERA_API_CLIENT_H

#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <tuple>
#include <vector>

#include "EIHCameraAPI.grpc.pb.h"
#include "eih_config.h"

namespace TmEIHCamera {

enum StatusCode { FAIL = -1, SUCCESS = 0 };

struct GrpcResult {
  StatusCode status;
  std::string error_message;
};

class EIHCameraApiClient {
 private:
  std::unique_ptr<EIHCameraApi::Stub> stub_;  // stub

 public:
  EIHCameraApiClient() = default;
  explicit EIHCameraApiClient(const std::string &server_address);

  bool isCameraConnected(GrpcResult &result,
                         tm_eih_config::IsCameraConnectedResponse &cam_connect);

  bool getIntrinsics(GrpcResult &result,
                     std::vector<tm_eih_config::Intrinsics> &intrinsics_res);
  bool getHandEyeParameters(GrpcResult &result,
                            tm_eih_config::HandEyeArray &hand_eye_array);

  bool getImageData(GrpcResult &result, tm_eih_config::Image::Data &byte_data);
  bool getImageConfiguration(GrpcResult &result,
                             tm_eih_config::Image::Configuration &image_config);

  // bool terminateCameraConnection(GrpcResult &result);
  // bool resumeCameraConnection(GrpcResult &result);

  bool getCapturingSettings(GrpcResult &result,
                            tm_eih_config::CapturingSettings &capturing_settings);
  bool setCapturingSettings(
      GrpcResult &result,
      const tm_eih_config::SetCapturingSettingsRequest &capturing_settings_req);

  bool getShutterTime(GrpcResult &result,
                      tm_eih_config::CaptureSettingValue &shutter_time);
  bool setShutterTime(GrpcResult &result, const int &shutter_time);

  bool getGain(GrpcResult &result, tm_eih_config::CaptureSettingValue &gain);
  bool setGain(GrpcResult &result, const int &gain);

  bool getWhiteBalance(GrpcResult &result,
                       tm_eih_config::WhiteBalance &white_balance);
  bool setWhiteBalance(GrpcResult &result,
                       tm_eih_config::SetWhiteBalanceRequest &white_balance_req);

  bool getFocus(GrpcResult &result, tm_eih_config::CaptureSettingValue &focus);
  bool setFocus(GrpcResult &result, const int &focus);

  bool getImageSize(
      GrpcResult &result,
      std::string &image_size);  // e.g., "1M:1280*960", "5M:2592*1944"
  bool setImageSize(GrpcResult &result, const std::string &image_size);
};

}  // namespace TmEIHCamera

#endif  // CAMERA_API_CLIENT_H
