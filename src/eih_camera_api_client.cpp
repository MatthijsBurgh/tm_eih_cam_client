#include "eih_camera_api_client.h"

#include <iostream>
#include <memory>

namespace TmEIHCamera {

EIHCameraApiClient::EIHCameraApiClient(const std::string &server_address) {
  grpc::ChannelArguments channel_args;
  channel_args.SetMaxReceiveMessageSize(
      -1);  // Set to unlimited size, or you can set a specific size in bytes.
  auto channel = grpc::CreateCustomChannel(
      server_address, grpc::InsecureChannelCredentials(), channel_args);
  stub_ = EIHCameraApi::NewStub(channel);
}

bool EIHCameraApiClient::isCameraConnected(
    GrpcResult &result, TmEIHConfig::IsCameraConnectedResponse &cam_connect) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::isCameraConnectedResponse response;
  grpc::Status status = stub_->isCameraConnected(&context, request, &response);

  if (status.ok()) {
    cam_connect.is_camera_connected = response.iscameraconnected();
    cam_connect.connection_message = response.connection_message();
    result.status = StatusCode::SUCCESS;
    result.error_message.clear();
    return true;
  } else {
    result.status = StatusCode::FAIL;
    result.error_message = status.error_message();
    return false;
  }
}

bool EIHCameraApiClient::getIntrinsics(
    GrpcResult &result, std::vector<TmEIHConfig::Intrinsics> &intrinsics_res) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::getIntrinsicsResponse response;
  grpc::Status status = stub_->getIntrinsics(&context, request, &response);

  if (status.ok()) {
    intrinsics_res.clear();
    for (const auto &intrinsics :      // TO DO: fix multi
         response.cam_intrinsics()) {  // if multi cameras
      TmEIHConfig::Intrinsics ci;
      ci.focus_value = intrinsics.focusvalue();
      ci.image_width = intrinsics.imagewidth();
      ci.image_height = intrinsics.imageheight();
      ci.camera_matrix.matrix_00 = intrinsics.cameramatrix().matrix_00();
      ci.camera_matrix.matrix_01 = intrinsics.cameramatrix().matrix_01();
      ci.camera_matrix.matrix_02 = intrinsics.cameramatrix().matrix_02();
      ci.camera_matrix.matrix_10 = intrinsics.cameramatrix().matrix_10();
      ci.camera_matrix.matrix_11 = intrinsics.cameramatrix().matrix_11();
      ci.camera_matrix.matrix_12 = intrinsics.cameramatrix().matrix_12();
      ci.camera_matrix.matrix_20 = intrinsics.cameramatrix().matrix_20();
      ci.camera_matrix.matrix_21 = intrinsics.cameramatrix().matrix_21();
      ci.camera_matrix.matrix_22 = intrinsics.cameramatrix().matrix_22();
      ci.distortion_coefficients.coefficient_00 =
          intrinsics.distortioncoefficients().coefficient_00();
      ci.distortion_coefficients.coefficient_10 =
          intrinsics.distortioncoefficients().coefficient_10();
      ci.distortion_coefficients.coefficient_20 =
          intrinsics.distortioncoefficients().coefficient_20();
      ci.distortion_coefficients.coefficient_30 =
          intrinsics.distortioncoefficients().coefficient_30();
      ci.distortion_coefficients.coefficient_40 =
          intrinsics.distortioncoefficients().coefficient_40();

      intrinsics_res.push_back(ci);
    }
    result.status = StatusCode::SUCCESS;
    result.error_message.clear();
    return true;
  } else {
    result.status = StatusCode::FAIL;
    result.error_message = status.error_message();
    return false;
  }
}
/*
bool EIHCameraApiClient::getHandEyeParameters(
    GrpcResult &result, TmEIHConfig::HandEyeArray &hand_eye_array) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::Camera_HandEyeParameters response;
  grpc::Status status =
      stub_->getHandEyeParameters(&context, request, &response);

  if (status.ok()) {
    // hand_eye_array = response.handeyearray();
    hand_eye_array.handeye_x = response.handeyearray().handeye_x();
    hand_eye_array.handeye_y = response.handeyearray().handeye_y();
    hand_eye_array.handeye_z = response.handeyearray().handeye_z();
    hand_eye_array.handeye_rx = response.handeyearray().handeye_rx();
    hand_eye_array.handeye_ry = response.handeyearray().handeye_ry();
    hand_eye_array.handeye_rz = response.handeyearray().handeye_rz();
    return true;
  } else {
    result.status = StatusCode::FAIL;
    result.error_message = status.error_message();
    return false;
  }
}
*/

bool EIHCameraApiClient::getImageData(GrpcResult &result,
                                      TmEIHConfig::Image::Data &data) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::Camera_Image_Data response;
  grpc::Status status = stub_->getImageData(&context, request, &response);

  if (status.ok()) {
    data.encode_string.assign(response.encodestring().begin(),
                              response.encodestring().end());
    result.status = StatusCode::SUCCESS;
    result.error_message.clear();
    return true;
  } else {
    data.encode_string.clear();
    result.status = StatusCode::FAIL;
    result.error_message = status.error_code();
    return false;
  }
}

bool EIHCameraApiClient::getImageConfiguration(
    GrpcResult &result, TmEIHConfig::Image::Configuration &image_config) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;

  TmEIHCamera::Camera_Image_Configuration response;
  grpc::Status status =
      stub_->getImageConfiguration(&context, request, &response);

  if (status.ok()) {
    image_config.image_type = response.imagetype();
    image_config.image_size = response.imagesize();
    image_config.image_width = response.imagewidth();
    image_config.image_height = response.imageheight();
    image_config.pixel_format = response.pixelformat();
    result.error_message.clear();
    return true;
  } else {
    result.status = StatusCode::FAIL;
    result.error_message = status.error_message();
    return false;
  }
}

/*
grpc::Status EIHCameraApiClient::terminateCameraConnection() {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  google::protobuf::Empty response;

  grpc::Status status =
      m_Stub->terminateCameraConnection(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Camera connection terminated successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::resumeCameraConnection() {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  google::protobuf::Empty response;

  grpc::Status status =
      m_Stub->resumeCameraConnection(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Camera connection resumed successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  return status;
}

grpc::Status EIHCameraApiClient::getCapturingSettings() {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  Camera_CapturingSettings response;

  grpc::Status status =
      m_Stub->getCapturingSettings(&context, request, &response);

  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Capturing settings retrieved successfully." << std::endl;
    std::cout << "Shutter Time - Current Value: "
              << response.shuttertime().currentvalue() << std::endl;
    std::cout << "Shutter Time - Min Value: "
              << response.shuttertime().minvalue() << std::endl;
    std::cout << "Shutter Time - Max Value: "
              << response.shuttertime().maxvalue() << std::endl;
    std::cout << "Gain - Current Value: " << response.gain().currentvalue()
              << std::endl;
    std::cout << "Gain - Min Value: " << response.gain().minvalue()
              << std::endl;
    std::cout << "Gain - Max Value: " << response.gain().maxvalue()
              << std::endl;
    std::cout << "Focus - Current Value: " << response.focus().currentvalue()
              << std::endl;
    std::cout << "Focus - Min Value: " << response.focus().minvalue()
              << std::endl;
    std::cout << "Focus - Max Value: " << response.focus().maxvalue()
              << std::endl;
    std::cout << "White Balance - Red Ratio - Current Value: "
              << response.whitebalance().red_ratio().currentvalue()
              << std::endl;
    std::cout << "White Balance - Red Ratio - Min Value: "
              << response.whitebalance().red_ratio().minvalue() << std::endl;
    std::cout << "White Balance - Red Ratio - Max Value: "
              << response.whitebalance().red_ratio().maxvalue() << std::endl;
    std::cout << "White Balance - Green Ratio - Current Value: "
              << response.whitebalance().green_ratio().currentvalue()
              << std::endl;
    std::cout << "White Balance - Green Ratio - Min Value: "
              << response.whitebalance().green_ratio().minvalue() << std::endl;
    std::cout << "White Balance - Green Ratio - Max Value: "
              << response.whitebalance().green_ratio().maxvalue() << std::endl;
    std::cout << "White Balance - Blue Ratio - Current Value: "
              << response.whitebalance().blue_ratio().currentvalue()
              << std::endl;
    std::cout << "White Balance - Blue Ratio - Min Value: "
              << response.whitebalance().blue_ratio().minvalue() << std::endl;
    std::cout << "White Balance - Blue Ratio - Max Value: "
              << response.whitebalance().blue_ratio().maxvalue() << std::endl;
    std::cout << "Image Size: " << response.imagesize() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }

  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::setCapturingSettings() {
  grpc::ClientContext context;
  TmEIHCamera::setCapturingSettingsRequest request;
  int _shuttertime, _gain, _wb_redratio, _wb_greenratio, _wb_blueratio, _focus;
  std::string _imagesize;
  std::cout << "Enter shutter time(134~66371): ";
  // std::cin >> _shuttertime;
  while (!(std::cin >> _shuttertime)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter gain(0~100): ";
  // std::cin >> _gain;
  while (!(std::cin >> _gain)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter wb_redratio(22~121): ";
  // std::cin >> _wb_redratio;
  while (!(std::cin >> _wb_redratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter wb_greenratio(1~1): ";
  // std::cin >> _wb_greenratio;
  while (!(std::cin >> _wb_greenratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter wb_blueratio(34~94): ";
  // std::cin >> _wb_blueratio;
  while (!(std::cin >> _wb_blueratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter focus(0~8): ";
  // std::cin >> _focus;
  while (!(std::cin >> _focus)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter imagesize(1M/5M): ";
  std::cin >> _imagesize;

  // Set request parameters
  request.set_shuttertime(_shuttertime);
  request.set_gain(_gain);
  request.set_wb_redratio(_wb_redratio);
  request.set_wb_greenratio(_wb_greenratio);
  request.set_wb_blueratio(_wb_blueratio);
  request.set_focus(_focus);
  request.set_imagesize(_imagesize);

  google::protobuf::Empty response;

  grpc::Status status =
      m_Stub->setCapturingSettings(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Capturing settings updated successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::getShutterTime() {
  grpc::ClientContext context;
  google::protobuf::Empty request;
  getShutterTimeResponse response;

  grpc::Status status = m_Stub->getShutterTime(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Shutter time retrieved successfully." << std::endl;
    std::cout << "Current value: " << response.shuttertime().currentvalue()
              << std::endl;
    std::cout << "Minimum value: " << response.shuttertime().minvalue()
              << std::endl;
    std::cout << "Maximum value: " << response.shuttertime().maxvalue()
              << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::setShutterTime() {
  grpc::ClientContext context;
  TmEIHCamera::setShutterTimeRequest request;
  int _shuttertime;
  std::cout << "Enter shutter time(134~66371): ";
  // std::cin >> _shuttertime;
  while (!(std::cin >> _shuttertime)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }

  // Set request parameter
  request.set_shuttertime(_shuttertime);

  google::protobuf::Empty response;

  grpc::Status status = m_Stub->setShutterTime(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Shutter time set successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::getGain() {
  grpc::ClientContext context;
  google::protobuf::Empty request;
  TmEIHCamera::getGainResponse response;

  grpc::Status status = m_Stub->getGain(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Gain retrieved successfully." << std::endl;
    std::cout << "Current value: " << response.gain().currentvalue()
              << std::endl;
    std::cout << "Minimum value: " << response.gain().minvalue() << std::endl;
    std::cout << "Maximum value: " << response.gain().maxvalue() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::setGain() {
  grpc::ClientContext context;
  TmEIHCamera::setGainRequest request;
  int _gain;
  std::cout << "Enter gain(0~100): ";
  // std::cin >> _gain;
  while (!(std::cin >> _gain)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }

  // Set request parameter
  request.set_gain(_gain);

  google::protobuf::Empty response;

  grpc::Status status = m_Stub->setGain(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Shutter time set successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::getWhiteBalance() {
  grpc::ClientContext context;
  google::protobuf::Empty request;
  TmEIHCamera::getWhiteBalanceResponse response;

  grpc::Status status = m_Stub->getWhiteBalance(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "White balance retrieved successfully." << std::endl;
    const TmEIHCamera::WhiteBalance &whiteBalance = response.whitebalance();
    std::cout << "Red Ratio: Current: "
              << whiteBalance.red_ratio().currentvalue()
              << ", Min: " << whiteBalance.red_ratio().minvalue()
              << ", Max: " << whiteBalance.red_ratio().maxvalue() << std::endl;
    std::cout << "Green Ratio: Current: "
              << whiteBalance.green_ratio().currentvalue()
              << ", Min: " << whiteBalance.green_ratio().minvalue()
              << ", Max: " << whiteBalance.green_ratio().maxvalue()
              << std::endl;
    std::cout << "Blue Ratio: Current: "
              << whiteBalance.blue_ratio().currentvalue()
              << ", Min: " << whiteBalance.blue_ratio().minvalue()
              << ", Max: " << whiteBalance.blue_ratio().maxvalue() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::setWhiteBalance() {
  grpc::ClientContext context;
  TmEIHCamera::setWhiteBalanceRequest request;
  int _wb_redratio, _wb_greenratio, _wb_blueratio;
  std::cout << "Enter wb_redratio(22~121): ";
  // std::cin >> _wb_redratio;
  while (!(std::cin >> _wb_redratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter wb_greenratio(1~1): ";
  // std::cin >> _wb_greenratio;
  while (!(std::cin >> _wb_greenratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }
  std::cout << "Enter wb_blueratio(34~94): ";
  // std::cin >> _wb_blueratio;
  while (!(std::cin >> _wb_blueratio)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }

  request.set_wb_redratio(_wb_redratio);
  request.set_wb_greenratio(_wb_greenratio);
  request.set_wb_blueratio(_wb_blueratio);

  google::protobuf::Empty response;
  grpc::Status status = m_Stub->setWhiteBalance(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "White balance set successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::getFocus() {
  grpc::ClientContext context;
  google::protobuf::Empty request;
  TmEIHCamera::getFocusResponse response;

  grpc::Status status = m_Stub->getFocus(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Focus parameters retrieved successfully." << std::endl;
    std::cout << "Current value: " << response.focus().currentvalue()
              << std::endl;
    std::cout << "Minimum value: " << response.focus().minvalue() << std::endl;
    std::cout << "Maximum value: " << response.focus().maxvalue() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}

grpc::Status EIHCameraApiClient::setFocus() {
  grpc::ClientContext context;
  TmEIHCamera::setFocusRequest request;
  int _focus;
  std::cout << "Enter focus(0~8): ";
  // std::cin >> _focus;
  while (!(std::cin >> _focus)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid argument. Please input a number again:";
  }

  // Set request parameter
  request.set_focus(_focus);

  google::protobuf::Empty response;

  grpc::Status status = m_Stub->setFocus(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Focus value set successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::getImageSize() {
  grpc::ClientContext context;
  google::protobuf::Empty request;
  TmEIHCamera::getImageSizeResponse response;
  grpc::Status status = m_Stub->getImageSize(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Image size retrieved successfully." << std::endl;
    std::cout << "Image size: " << response.imagesize() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
grpc::Status EIHCameraApiClient::setImageSize() {
  grpc::ClientContext context;
  TmEIHCamera::setImageSizeRequest request;
  std::string _imagesize;
  std::cout << "Enter imagesize(1M/5M): ";
  std::cin >> _imagesize;

  // Set request parameter
  request.set_imagesize(_imagesize);

  google::protobuf::Empty response;

  grpc::Status status = m_Stub->setImageSize(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Image size set successfully." << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
*/
}  // namespace TmEIHCamera
