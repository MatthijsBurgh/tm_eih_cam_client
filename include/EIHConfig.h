#ifndef EIHCONFIG_H
#define EIHCONFIG_H

#include <string>

namespace TmEIHCamera {

// proto message format
struct HandEyeParameters {
  float handeye_x;
  float handeye_y;
  float handeye_z;
  float handeye_rx;
  float handeye_ry;
  float handeye_rz;
};

/*
struct CaptureSetting_value {
  int CurrentValue;
  int MinValue;
  int MaxValue;
};

struct WhiteBalance {
  CaptureSetting_value red_ratio;
  CaptureSetting_value green_ratio;
  CaptureSetting_value blue_ratio;
};
*/

struct ImageConfiguration {
  std::string ImageType;  // Only support'png'
  std::string ImageSize;  // 1M:1280*960, 5M:2592*1944
  int ImageWidth;
  int ImageHeight;
  std::string PixelFormat;  // MONO, RGB
};

/*
struct CapturingSettings {
  CaptureSetting_value ShutterTime;
  CaptureSetting_value Gain;
  WhiteBalance WhiteBalance;
  CaptureSetting_value Focus;
  std::string ImageSize;  // 1M:1280*960, 5M:2592*1944
};
*/

struct CapturingSettings {
  TmEIHCamera::CaptureSetting_value ShutterTime;
  TmEIHCamera::CaptureSetting_value Gain;
  TmEIHCamera::WhiteBalance WhiteBalance;
  TmEIHCamera::CaptureSetting_value Focus;
  std::string ImageSize;  // 1M:1280*960, 5M:2592*1944
};

}  // namespace TmEIHCamera

#endif  // EIHCONFIG_H