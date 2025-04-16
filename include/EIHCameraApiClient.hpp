#pragma once
#ifndef CAMERA_API_CLIENT_H
#define CAMERA_API_CLIENT_H

#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <tuple>
#include <vector>

#include "EIHCameraAPI.grpc.pb.h"

namespace TmEIHCamera {

class EIHCameraApiClient {
 private:
  std::unique_ptr<EIHCameraApi::Stub> m_Stub;  // stub

 public:
  EIHCameraApiClient() = default;
  explicit EIHCameraApiClient(const std::string &server_address);

  bool isCameraConnected();
  std::vector<unsigned char> getImageData();
  // to do
  std::tuple<std::string, std::string, int, int, std::string>
  getImageConfiguration();

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
