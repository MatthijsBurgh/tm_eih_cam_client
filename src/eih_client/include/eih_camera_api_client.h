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
                         TmEIHConfig::CameraConnection &cam_connect);
  bool getImageData(GrpcResult &result, std::vector<unsigned char> &byte_data);
  bool getImageConfiguration(GrpcResult &result,
                             TmEIHConfig::Image::Configuration &image_config);

  // bool getHandEyeParameters(GrpcResult &result,
  //                           TmEIHConfig::HandEyeArray &hand_eye_array);

  //   grpc::Status isCameraConnected();
  //   grpc::Status getIntrinsics();
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
