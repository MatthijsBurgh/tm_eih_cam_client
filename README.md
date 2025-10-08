# tm_eih_cam_client

A ROS 2 package that bridges the TM Robot EIH camera gRPC API (TMflow ≥ 2.20) into ROS 2.
It fetches images and camera parameters from the EIH camera, republishes them to ROS 2 topics, and allows runtime configuration via ROS 2 parameters.

For more details about the TM EIH camera API, you can refer to the document:
[EIH Camera API Function Manual](https://www.tm-robot.com/zh-hant/download-center/?q=eih&catid=3100&theme=table&limit=5#3100-5372-wpfd-2-20-eih-camera-api)

---

## Features

- Uses gRPC client to call the EIH camera API.
- Publishes:
  - `/eih_camera/image_raw` (sensor_msgs/msg/Image)
  - `/eih_camera/image_raw/compressed` (sensor_msgs/msg/CompressedImage)
  - `/eih_camera/camera_info` (sensor_msgs/msg/CameraInfo)
- Exposes camera parameters via ROS 2 parameters so you can change camera settings at runtime.

---

## Requirements

- ROS 2 Humble
- [gRPC](https://github.com/grpc/grpc)
- OpenCV

A Dockerfile is provided as a reference for setting up the environment.

---

## Build

Build the workspace (inside a container or on native host):

   - Build with Docker (optional):
      ```bash
      docker build -t <your_image_name> .
      ```
      Example: `docker build -t tm_eih_cam_client .`

   - Inside the container or on host, build the package:
     ```bash
     colcon build
     source install/setup.bash
     ```
## Quickstart

  - Start the node with your robot IP:
    ```bash
    ros2 launch tm_eih_cam_client eih_client_pub.launch.py robot_ip:=192.168.x.x
    ```

    You can set robot IP in eih_client_pub.launch.py file

   - show image
      ```bash
      ros2 run tm_eih_cam_client image_sub
      ```