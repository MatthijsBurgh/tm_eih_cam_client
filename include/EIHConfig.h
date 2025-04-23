#ifndef EIHCONFIG_H
#define EIHCONFIG_H

#include <string>
// ref from EIHCamera.proto format
namespace TmEIHConfig {

struct CameraConnection {
  bool is_connected;
  std::string connection_message;
};

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
  int CurrentValue;
  int MinValue;
  int MaxValue;
};

struct WhiteBalance {
  CaptureSettingValue red_ratio;
  CaptureSettingValue green_ratio;
  CaptureSettingValue blue_ratio;
};

// to do
struct Camera {
  std::string SerialNumber;
};

struct Image {
  struct Configuration {
    std::string ImageType;  // Only support'png'
    std::string ImageSize;  // 1M:1280*960, 5M:2592*1944
    int ImageWidth;
    int ImageHeight;
    std::string PixelFormat;  // MONO, RGB
  };

  std::vector<unsigned char> byte_data;  // img raw data
  Configuration config;
};

// struct CapturingSettings {
//   CaptureSettingValue ShutterTime;
//   CaptureSettingValue Gain;
//   WhiteBalance WhiteBalance;
//   CaptureSettingValue Focus;
//   std::string ImageSize;  // 1M:1280*960, 5M:2592*1944
// };

}  // namespace TmEIHConfig

#endif  // EIHCONFIG_H