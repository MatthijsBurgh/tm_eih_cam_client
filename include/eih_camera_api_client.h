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

  // to do namespace & connection_message
  bool isCameraConnected(GrpcResult &result,
                         TmEIHConfig::IsCameraConnectedResponse &cam_connect);

  bool getIntrinsics(GrpcResult &result,
                     TmEIHConfig::GetGainResponse &gain_res);
  bool getHandEyeParameters(GrpcResult &result,
                            TmEIHConfig::HandEyeArray &hand_eye_array);

  bool getImageData(GrpcResult &result,
                    TmEIHConfig::Camera::Image::Data &byte_data);
  bool getImageConfiguration(
      GrpcResult &result,
      TmEIHConfig::Camera::Image::Configuration &image_config);

  // bool terminateCameraConnection(GrpcResult &result);
  // bool resumeCameraConnection(GrpcResult &result);

  // bool getCapturingSettings();
  // bool setCapturingSettings();

  // bool getShutterTime();
  // bool setShutterTime();

  // bool getGain();
  // bool setGain();

  // bool getWhiteBalance();
  // bool setWhiteBalance();

  // bool getFocus();
  // bool setFocus();

  // bool getImageSize();
  // bool setImageSize();

  //   grpc::Status isCameraConnected();
  //   grpc::Status getHandEyeParameters();
  //   grpc::Status getImageData();
  //   grpc::Status getImageConfiguration();
  //   grpc::Status terminateCameraConnection();
  //   grpc::Status resumeCameraConnection();
  //   grpc::Status getCapturingSettings();
  //   grpc::Status setCapturingSettings();  //
  //   grpc::Status getShutterTime();
  //   grpc::Status setShutterTime();  //
  //   grpc::Status getGain();
  //   grpc::Status setGain();  //
  //   grpc::Status getWhiteBalance();
  //   grpc::Status setWhiteBalance();  //
  //   grpc::Status getFocus();
  //   grpc::Status setFocus();  //
  //   grpc::Status getImageSize();
  //   grpc::Status setImageSize();  //
};

}  // namespace TmEIHCamera

#endif  // CAMERA_API_CLIENT_H
