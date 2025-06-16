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
                         TmEIHConfig::IsCameraConnectedResponse &cam_connect);

  bool getIntrinsics(GrpcResult &result,
                     std::vector<TmEIHConfig::Intrinsics> &intrinsics_res);
  bool getHandEyeParameters(GrpcResult &result,
                            TmEIHConfig::HandEyeArray &hand_eye_array);

  bool getImageData(GrpcResult &result, TmEIHConfig::Image::Data &byte_data);
  bool getImageConfiguration(GrpcResult &result,
                             TmEIHConfig::Image::Configuration &image_config);

  // bool terminateCameraConnection(GrpcResult &result);
  // bool resumeCameraConnection(GrpcResult &result);

  bool getCapturingSettings(GrpcResult &result,
                            TmEIHConfig::CapturingSettings &capturing_settings);
  bool setCapturingSettings(
      GrpcResult &result,
      const TmEIHConfig::SetCapturingSettingsRequest &capturing_settings_req);

  bool getShutterTime(GrpcResult &result,
                      TmEIHConfig::CaptureSettingValue &shutter_time);
  bool setShutterTime(GrpcResult &result, const int &shutter_time);

  bool getGain(GrpcResult &result, TmEIHConfig::CaptureSettingValue &gain);
  // bool setGain();

  bool getWhiteBalance(GrpcResult &result,
                       TmEIHConfig::WhiteBalance &white_balance);
  // bool setWhiteBalance();

  bool getFocus(GrpcResult &result, TmEIHConfig::CaptureSettingValue &focus);
  // bool setFocus();

  bool getImageSize(
      GrpcResult &result,
      std::string &image_size);  // e.g., "1M:1280*960", "5M:2592*1944"
  // bool setImageSize();
};

}  // namespace TmEIHCamera

#endif  // CAMERA_API_CLIENT_H
