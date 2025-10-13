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

  - Create a new workspace and clone the package:
      ```bash
      mkdir -p ~/tm_eih_ws/src

      cd ~/tm_eih_ws/src
      
      git clone https://github.com/TechmanRobotInc/tm_eih_cam_client.git
      ```
  - You can build and run this package either directly on your host system or inside Docker for a consistent development environment.

### Option 1: Native Build (on Host)

  1. Ensure ROS 2 Humble and dependencies are installed and sourced.

  2. Build the workspace:
      ```bash
      cd ~/tm_eih_ws

      colcon build

      source install/setup.bash
      ```

### Option 2: Docker Build
  
  1. Define your image name:
      ```bash
      export IMAGE_NAME=tm_eih_cam_client
      ```

  2. Build the Docker image:
      ```bash
      cd ~/tm_eih_ws/src/tm_eih_cam_client

      docker build -t $IMAGE_NAME .
      ```

  3. Start an development container:
      ```bash
      docker run -it --rm -v $(pwd):/workspaces/src -w /workspaces $IMAGE_NAME /bin/bash
      ```

  4. Inside the container:
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

   <!-- - show image
      ```bash
      ros2 run tm_eih_cam_client image_sub
      ``` -->