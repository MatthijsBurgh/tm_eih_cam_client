#include "EIHCameraApiClient.hpp"

#include <iostream>
#include <memory>

namespace TmEIHCamera {

EIHCameraApiClient::EIHCameraApiClient(const std::string &server_address) {
  grpc::ChannelArguments channel_args;
  channel_args.SetMaxReceiveMessageSize(
      -1);  // Set to unlimited size, or you can set a specific size in bytes.
  auto channel = grpc::CreateCustomChannel(
      server_address, grpc::InsecureChannelCredentials(), channel_args);
  _stub = EIHCameraApi::NewStub(channel);
}

bool EIHCameraApiClient::isCameraConnected(GrpcResult &result,
                                           bool &is_connected) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::isCameraConnectedResponse response;

  grpc::Status status = _stub->isCameraConnected(&context, request, &response);
  is_connected = response.iscameraconnected();

  if (status.ok()) {
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
grpc::Status EIHCameraApiClient::getIntrinsics() {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::getIntrinsicsResponse response;

  grpc::Status status = m_Stub->getIntrinsics(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Intrinsics retrieved successfully:" << std::endl;
    for (const auto &intrinsics : response.cam_intrinsics()) {
      std::cout << "��=============================��" << std::endl;
      std::cout << "Camera Matrix:" << std::endl;
      std::cout << "  Matrix_00: " << intrinsics.cameramatrix().matrix_00()
                << std::endl;
      std::cout << "  Matrix_01: " << intrinsics.cameramatrix().matrix_01()
                << std::endl;
      std::cout << "  Matrix_02: " << intrinsics.cameramatrix().matrix_02()
                << std::endl;
      std::cout << "  Matrix_10: " << intrinsics.cameramatrix().matrix_10()
                << std::endl;
      std::cout << "  Matrix_11: " << intrinsics.cameramatrix().matrix_11()
                << std::endl;
      std::cout << "  Matrix_12: " << intrinsics.cameramatrix().matrix_12()
                << std::endl;
      std::cout << "  Matrix_20: " << intrinsics.cameramatrix().matrix_20()
                << std::endl;
      std::cout << "  Matrix_21: " << intrinsics.cameramatrix().matrix_21()
                << std::endl;
      std::cout << "  Matrix_22: " << intrinsics.cameramatrix().matrix_22()
                << std::endl;

      std::cout << "Distortion Coefficients:" << std::endl;
      std::cout << "  Coefficient_00: "
                << intrinsics.distortioncoefficients().coefficient_00()
                << std::endl;
      std::cout << "  Coefficient_10: "
                << intrinsics.distortioncoefficients().coefficient_10()
                << std::endl;
      std::cout << "  Coefficient_20: "
                << intrinsics.distortioncoefficients().coefficient_20()
                << std::endl;
      std::cout << "  Coefficient_30: "
                << intrinsics.distortioncoefficients().coefficient_30()
                << std::endl;
      std::cout << "  Coefficient_40: "
                << intrinsics.distortioncoefficients().coefficient_40()
                << std::endl;

      std::cout << "Focus Value: " << intrinsics.focusvalue() << std::endl;
      std::cout << "Image Width: " << intrinsics.imagewidth() << std::endl;
      std::cout << "Image Height: " << intrinsics.imageheight() << std::endl;
      std::cout << "��=============================��" << std::endl;
    }
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;
  return status;
}

grpc::Status EIHCameraApiClient::getHandEyeParameters() {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::Camera_HandEyeParameters response;

  grpc::Status status =
      m_Stub->getHandEyeParameters(&context, request, &response);
  std::cout << "----------------------------------------------------------"
            << std::endl;
  if (status.ok()) {
    std::cout << "Hand-eye parameters retrieved successfully." << std::endl;
    const TmEIHCamera::HandEyeArray &handEyeArray = response.handeyearray();
    std::cout << "Hand-eye array: " << std::endl;
    std::cout << "  handeye_x: " << handEyeArray.handeye_x() << std::endl;
    std::cout << "  handeye_y: " << handEyeArray.handeye_y() << std::endl;
    std::cout << "  handeye_z: " << handEyeArray.handeye_z() << std::endl;
    std::cout << "  handeye_rx: " << handEyeArray.handeye_rx() << std::endl;
    std::cout << "  handeye_ry: " << handEyeArray.handeye_ry() << std::endl;
    std::cout << "  handeye_rz: " << handEyeArray.handeye_rz() << std::endl;
  } else {
    std::cout << "RPC failed: " << status.error_code() << ": "
              << status.error_message() << std::endl;
  }
  std::cout << "----------------------------------------------------------"
            << std::endl;

  return status;
}
*/

bool EIHCameraApiClient::getImageData(GrpcResult &result,
                                      std::vector<unsigned char> &byte_data) {
  grpc::ClientContext context;
  const google::protobuf::Empty request;
  TmEIHCamera::Camera_Image_Data response;
  grpc::Status status = _stub->getImageData(&context, request, &response);

  if (status.ok()) {
    byte_data.assign(response.encodestring().begin(),
                     response.encodestring().end());
    result.status = StatusCode::SUCCESS;
    result.error_message.clear();
    return true;
  } else {
    byte_data.clear();
    result.status = StatusCode::FAIL;
    result.error_message = status.error_code();
    return false;
  }
}

// std::tuple<std::string, std::string, int, int, std::string>
// EIHCameraApiClient::getImageConfiguration() {
//   grpc::ClientContext context;
//   const google::protobuf::Empty request;

//   TmEIHCamera::Camera_Image_Configuration response;
//   grpc::Status status =
//       _stub->getImageConfiguration(&context, request, &response);
//   std::cout << "----------------------------------------------------------"
//             << std::endl;
//   if (status.ok()) {
//     // std::cout << "Image configuration retrieved successfully:" <<
//     std::endl;
//     // std::cout << "  Image Type: " << response.imagetype() << std::endl;
//     // std::cout << "  Image Size: " << response.imagesize() << std::endl;
//     // std::cout << "  Image Width: " << response.imagewidth() << std::endl;
//     // std::cout << "  Image Height: " << response.imageheight() <<
//     std::endl;
//     // std::cout << "  Pixel Format: " << response.pixelformat() <<
//     std::endl;
//     // std::cout <<
//     "----------------------------------------------------------"
//     //           << std::endl;
//     return std::make_tuple(response.imagetype(), response.imagesize(),
//                            response.imagewidth(), response.imageheight(),
//                            response.pixelformat());
//   } else {
//     std::cout << "RPC failed: " << status.error_code() << ": "
//               << status.error_message() << std::endl;
//     std::cout << "----------------------------------------------------------"
//               << std::endl;
//     return std::make_tuple("", "", 0, 0, "");
//   }
// }

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
