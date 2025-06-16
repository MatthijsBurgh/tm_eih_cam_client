#ifndef EIHCONFIG_H
#define EIHCONFIG_H

#include <cstdint>
#include <string>
#include <vector>

// ref from EIHCamera.proto format
namespace TmEIHConfig {

struct CameraMatrix {
  float matrix_00 = -1;
  float matrix_01 = -1;
  float matrix_02 = -1;
  float matrix_10 = -1;
  float matrix_11 = -1;
  float matrix_12 = -1;
  float matrix_20 = -1;
  float matrix_21 = -1;
  float matrix_22 = -1;
};

struct DistortionCoefficients {
  float coefficient_00 = -1;
  float coefficient_10 = -1;
  float coefficient_20 = -1;
  float coefficient_30 = -1;
  float coefficient_40 = -1;
};

struct HandEyeArray {
  float handeye_x = -1;
  float handeye_y = -1;
  float handeye_z = -1;
  float handeye_rx = -1;
  float handeye_ry = -1;
  float handeye_rz = -1;
};

struct CaptureSettingValue {
  int current_value = -1;
  int min_value = -1;
  int max_value = -1;
};

struct WhiteBalance {
  CaptureSettingValue red_ratio;
  CaptureSettingValue green_ratio;
  CaptureSettingValue blue_ratio;
};

#ifdef ORIGINAL_PROTOBUF_STRUCT
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
      std::vector<uint8_t> encode_string;  // image raw data
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
  };  // Image

  struct CapturingSettings {
    CaptureSettingValue shutter_time;
    CaptureSettingValue gain;
    WhiteBalance white_balance;
    CaptureSettingValue focus;
    std::string image_size;  // 1M:1280*960, 5M:2592*1944
  };

  std::vector<Intrinsics> intrinsics;  // repeated
  HandEyeParameters hand_eye_parameters;
  CapturingSettings capturing_settings;
  Image image;
};  // Camera

#else
// struct Information {
//   std::string serial_number;
// };

struct Intrinsics {
  float focus_value = -1;
  int image_width = -1;
  int image_height = -1;
  CameraMatrix camera_matrix;
  DistortionCoefficients distortion_coefficients;
};

// struct HandEyeParameters {
//   HandEyeArray hand_eye_array;
// };

struct Image {
  struct Data {
    std::vector<uint8_t> encode_string;  // image raw data
  };
  struct Configuration {
    std::string image_type;  // Only support 'png'
    std::string image_size;  // 1M:1280*960, 5M:2592*1944
    int image_width = -1;
    int image_height = -1;
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

#endif

// ref from EIHCameraAPI.proto ------------
struct IsCameraConnectedResponse {
  bool is_camera_connected = 0;
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
// struct SetShutterTimeRequest {
//   int shutter_time;
// };

struct GetGainResponse {
  CaptureSettingValue gain;
};
// struct SetGainRequest {
//   int gain;
// };

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
// struct SetFocusRequest {
//   int focus;
// };

struct GetImageSizeResponse {
  std::string image_size;  // 1M:1280_960, 5M:2592_1944
};
// struct SetImageSizeRequest {
//   std::string image_size;  // 1M:1280_960, 5M:2592_1944
// };

// struct GetIntrinsicsResponse {
//   Camera::Intrinsics cam_intrinsics;
// };

}  // namespace TmEIHConfig

#endif  // EIHCONFIG_H