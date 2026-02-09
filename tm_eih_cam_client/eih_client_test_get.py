#!/usr/bin/env python3
"""Test script to get camera settings and data

This script demonstrates getting various camera settings and image data.
"""

import sys
import cv2
import numpy as np
from tm_eih_cam_client.eih_camera_api_client import EIHCameraApiClient


def main():
    if len(sys.argv) < 2:
        print("Usage: ros2 run tm_eih_cam_client eih_client_test_get <robot_ip>")
        print("Example: ros2 run tm_eih_cam_client eih_client_test_get 192.168.10.20")
        return 1
    
    robot_ip = sys.argv[1]
    server_address = f"{robot_ip}:15567"
    
    print(f"Connecting to EIH camera at {server_address}")
    client = EIHCameraApiClient(server_address)
    
    # Test connection
    print("\n=== Testing camera connection ===")
    result, conn_info = client.is_camera_connected()
    if result.status == 0 and conn_info:
        print(f"Camera connected: {conn_info['is_camera_connected']}")
        print(f"Connection message: {conn_info['connection_message']}")
    else:
        print(f"Failed to check connection: {result.error_message}")
        return 1
    
    # Get capturing settings
    print("\n=== Getting capturing settings ===")
    result, settings = client.get_capturing_settings()
    if result.status == 0 and settings:
        print(f"Shutter time: {settings['shutter_time']['current_value']} "
              f"({settings['shutter_time']['min_value']}-{settings['shutter_time']['max_value']})")
        print(f"Gain: {settings['gain']['current_value']} "
              f"({settings['gain']['min_value']}-{settings['gain']['max_value']})")
        print(f"Focus: {settings['focus']['current_value']} "
              f"({settings['focus']['min_value']}-{settings['focus']['max_value']})")
        print(f"Image size: {settings['image_size']}")
        print(f"White balance - Red: {settings['white_balance']['red_ratio']['current_value']}")
        print(f"White balance - Green: {settings['white_balance']['green_ratio']['current_value']}")
        print(f"White balance - Blue: {settings['white_balance']['blue_ratio']['current_value']}")
    else:
        print(f"Failed to get settings: {result.error_message}")
    
    # Get intrinsics
    print("\n=== Getting camera intrinsics ===")
    result, intrinsics = client.get_intrinsics()
    if result.status == 0 and intrinsics:
        print(f"Found {len(intrinsics)} intrinsic calibrations")
        for i, intr in enumerate(intrinsics[:3]):  # Show first 3
            print(f"\nCalibration {i+1}:")
            print(f"  Focus: {intr['focus_value']}")
            print(f"  Resolution: {intr['image_width']}x{intr['image_height']}")
    else:
        print(f"Failed to get intrinsics: {result.error_message}")
    
    # Get hand-eye parameters
    print("\n=== Getting hand-eye parameters ===")
    result, hand_eye = client.get_hand_eye_parameters()
    if result.status == 0 and hand_eye:
        print(f"Hand-eye transform:")
        print(f"  Position: ({hand_eye['handeye_x']:.3f}, "
              f"{hand_eye['handeye_y']:.3f}, {hand_eye['handeye_z']:.3f})")
        print(f"  Rotation: ({hand_eye['handeye_rx']:.3f}, "
              f"{hand_eye['handeye_ry']:.3f}, {hand_eye['handeye_rz']:.3f})")
    else:
        print(f"Failed to get hand-eye parameters: {result.error_message}")
    
    # Get image configuration
    print("\n=== Getting image configuration ===")
    result, config = client.get_image_configuration()
    if result.status == 0 and config:
        print(f"Image type: {config['image_type']}")
        print(f"Image size: {config['image_size']}")
        print(f"Resolution: {config['image_width']}x{config['image_height']}")
        print(f"Pixel format: {config['pixel_format']}")
    else:
        print(f"Failed to get image configuration: {result.error_message}")
    
    # Get and display image
    print("\n=== Getting image data ===")
    result, image_data = client.get_image_data()
    if result.status == 0 and image_data:
        print(f"Received {len(image_data)} bytes of image data")
        
        # Decode and display
        np_arr = np.frombuffer(image_data, np.uint8)
        img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        
        if img is not None:
            print(f"Image decoded: {img.shape}")
            cv2.imshow('EIH Camera Image', img)
            print("Press any key to close...")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        else:
            print("Failed to decode image")
    else:
        print(f"Failed to get image data: {result.error_message}")
    
    print("\n=== Test completed ===")
    return 0


if __name__ == '__main__':
    sys.exit(main())
