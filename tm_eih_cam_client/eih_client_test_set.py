#!/usr/bin/env python3
"""Test script to set camera settings

This script demonstrates setting various camera settings.
"""

import sys
from tm_eih_cam_client.eih_camera_api_client import EIHCameraApiClient


def main():
    if len(sys.argv) < 2:
        print("Usage: ros2 run tm_eih_cam_client eih_client_test_set <robot_ip>")
        print("Example: ros2 run tm_eih_cam_client eih_client_test_set 192.168.10.20")
        return 1
    
    robot_ip = sys.argv[1]
    server_address = f"{robot_ip}:15567"
    
    print(f"Connecting to EIH camera at {server_address}")
    client = EIHCameraApiClient(server_address)
    
    # Test setting individual parameters
    print("\n=== Testing individual parameter settings ===")
    
    # Set focus
    print("\nSetting focus to 4...")
    result = client.set_focus(4)
    if result.status != 0:
        print(f"Failed: {result.error_message}")
    
    # Set gain
    print("Setting gain to 10...")
    result = client.set_gain(10)
    if result.status != 0:
        print(f"Failed: {result.error_message}")
    
    # Set shutter time
    print("Setting shutter time to 10000...")
    result = client.set_shutter_time(10000)
    if result.status != 0:
        print(f"Failed: {result.error_message}")
    
    # Set white balance
    print("Setting white balance (R:62, G:1, B:51)...")
    result = client.set_white_balance(62, 1, 51)
    if result.status != 0:
        print(f"Failed: {result.error_message}")
    
    # Test setting all parameters at once
    print("\n=== Testing batch parameter setting ===")
    print("Setting all capturing parameters...")
    result = client.set_capturing_settings(
        shutter_time=36088,
        gain=4,
        wb_redratio=62,
        wb_greenratio=1,
        wb_blueratio=51,
        focus=6,
        image_size="1M"
    )
    
    if result.status == 0:
        print("Successfully set all parameters")
    else:
        print(f"Failed to set parameters: {result.error_message}")
    
    # Verify settings
    print("\n=== Verifying settings ===")
    result, settings = client.get_capturing_settings()
    if result.status == 0 and settings:
        print(f"Current settings:")
        print(f"  Shutter time: {settings['shutter_time']['current_value']}")
        print(f"  Gain: {settings['gain']['current_value']}")
        print(f"  Focus: {settings['focus']['current_value']}")
        print(f"  Image size: {settings['image_size']}")
        print(f"  WB Red: {settings['white_balance']['red_ratio']['current_value']}")
        print(f"  WB Green: {settings['white_balance']['green_ratio']['current_value']}")
        print(f"  WB Blue: {settings['white_balance']['blue_ratio']['current_value']}")
    
    print("\n=== Test completed ===")
    return 0


if __name__ == '__main__':
    sys.exit(main())
