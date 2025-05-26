#ifndef EIHCONFIG_H
#define EIHCONFIG_H

#include <string>
#include <vector>
#include <cstdint>

// ref from EIHCamera.proto format
namespace TmEIHConfig {

struct CameraMatrix {
  float matrix_00;
  float matrix_01;
  float matrix_02;
  float matrix_10;
  float matrix_11;
  float matrix_12;
  float matrix_20;
  float matrix_21;
  float matrix_22;
};

struct DistortionCoefficients {
  float coefficient_00;
  float coefficient_10;
  float coefficient_20;
  float coefficient_30;
  float coefficient_40;
};

struct HandEyeArray {
  float handeye_x;
  float handeye_y;
  float handeye_z;
  float handeye_rx;
  float handeye_ry;
  float handeye_rz;
};

struct CaptureSettingValue {
  int current_value;
  int min_value;
  int max_value;
};

struct WhiteBalance {
  CaptureSettingValue red_ratio;
  CaptureSettingValue green_ratio;
  CaptureSettingValue blue_ratio;
};

struct Camera {

  struct Information {
    std::string serial_number;
  };
  struct Intrinsics {
    float focus_value;
    int image_width;
    int image_height;
    CameraMatrix camera_matrix;
    DistortionCoefficients distortion_coefficients;
  };

  struct HandEyeParameters {
    HandEyeArray hand_eye_array;
  };

  struct Image {
  
    struct Data {
      std::vector<uint8_t> encode_string;  // img raw data
    };
    struct Configuration {
      std::string image_type;  // Only support'png'
      std::string image_size;  // 1M:1280*960, 5M:2592*1944
      int image_width;
      int image_height;
      std::string pixel_format;  // MONO, RGB
    };
  
    Data data;
    Configuration config;
  };
  struct CapturingSettings {
    CaptureSettingValue shutter_time;
    CaptureSettingValue gain;
    WhiteBalance white_balance;
    CaptureSettingValue focus;
    std::string image_size;  // 1M:1280*960, 5M:2592*1944
  };

  std::vector<Intrinsics> intrinsics; // repeated
  HandEyeParameters handeye_parameters;
  CapturingSettings capturing_settings;
  Image image;
};




// ref from EIHCameraAPI.proto ------------
struct SerialNumberRequest {
  std::string serial_number;
};
struct IsCameraConnection {
  bool is_connected;
  std::string connection_message;
};

struct SetCapturingSettingsRequest {
  int shutter_time;
  int gain;
  int wb_redratio;
  int wb_greenratio;
  int wb_blueratio;
  int focus;
  std::string image_size;  // 1M = 1280*960,  5M = 2592*1944
};

struct GetShutterTimeResponse {
  CaptureSettingValue shutter_time;
};
struct SetShutterTimeRequest {
  int shutter_time;
};

struct GetGainResponse {
  CaptureSettingValue gain;
};
struct SetGainRequest {
  int gain;
};

struct GetWhiteBalanceResponse {
  WhiteBalance white_balance;
};
struct SetWhiteBalanceRequest {
  int wb_redratio;
  int wb_greenratio;
  int wb_blueratio;
};

struct GetFocusResponse {
  CaptureSettingValue focus;
};
struct SetFocusRequest {
  int focus;
};

struct GetImageSizeResponse {
  std::string image_size;  // 1M:1280_960, 5M:2592_1944
};
struct SetImageSizeRequest {
  std::string image_size;  // 1M:1280_960, 5M:2592_1944
};

}  // namespace TmEIHConfig

#endif  // EIHCONFIG_H