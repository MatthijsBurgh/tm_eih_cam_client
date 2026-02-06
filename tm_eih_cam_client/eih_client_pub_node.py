#!/usr/bin/env python3
"""EIH Client Publisher Node

This node publishes images and camera info from the TM Robot EIH camera.
"""

import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter
from rcl_interfaces.msg import SetParametersResult
from sensor_msgs.msg import Image, CameraInfo, CompressedImage
from std_msgs.msg import Header
import cv2
import numpy as np
from cv_bridge import CvBridge
import time

from tm_eih_cam_client.eih_camera_api_client import EIHCameraApiClient


class EIHClientPublisher(Node):
    """ROS 2 node for publishing EIH camera data"""
    
    def __init__(self):
        super().__init__('eih_client_publisher')
        
        # Declare parameters
        self.declare_parameter('robot_ip', '')
        self.declare_parameter('frame_id', 'eih_camera')
        self.declare_parameter('image_encoding', 'bgr8')
        
        self.declare_parameter('shutter_time', -1)
        self.declare_parameter('gain', -1)
        self.declare_parameter('wb_redratio', -1)
        self.declare_parameter('wb_greenratio', -1)
        self.declare_parameter('wb_blueratio', -1)
        self.declare_parameter('focus', -1)
        self.declare_parameter('image_size', '')
        
        # Get parameters
        robot_ip = self.get_parameter('robot_ip').value
        self.frame_id = self.get_parameter('frame_id').value
        self.image_encoding = self.get_parameter('image_encoding').value
        
        # Create gRPC client
        camera_addr = f"{robot_ip}:15567"
        self.get_logger().info(f"Connecting to EIH camera at {camera_addr}")
        self.client = EIHCameraApiClient(camera_addr)
        
        # Initialize camera parameters
        self._get_eih_params()
        self._set_params_to_eih_camera()
        
        # Register parameter callback
        self.add_on_set_parameters_callback(self._on_params_change)
        
        # Create publishers
        self.image_pub = self.create_publisher(
            Image, '/eih_camera/image_raw', 10)
        self.compressed_pub = self.create_publisher(
            CompressedImage, '/eih_camera/image_raw/compressed', 10)
        self.camera_info_pub = self.create_publisher(
            CameraInfo, '/eih_camera/camera_info', 10)
        
        # Create timer for image publishing (30 Hz)
        self.timer = self.create_timer(0.033, self._publish_image)
        
        # Create CV Bridge
        self.bridge = CvBridge()
        
        self.get_logger().info("EIH Client Publisher initialized")
    
    def _get_eih_params(self):
        """Get EIH camera parameters from ROS parameters"""
        self.camera_params = {
            'shutter_time': self.get_parameter('shutter_time').value,
            'gain': self.get_parameter('gain').value,
            'wb_redratio': self.get_parameter('wb_redratio').value,
            'wb_greenratio': self.get_parameter('wb_greenratio').value,
            'wb_blueratio': self.get_parameter('wb_blueratio').value,
            'focus': self.get_parameter('focus').value,
            'image_size': self.get_parameter('image_size').value,
        }
    
    def _on_params_change(self, params):
        """Callback when parameters are changed"""
        result = SetParametersResult()
        result.successful = True
        
        # Update camera parameters
        for param in params:
            if param.name in self.camera_params:
                self.camera_params[param.name] = param.value
        
        # Apply settings to camera
        self._set_params_to_eih_camera()
        
        return result
    
    def _set_params_to_eih_camera(self):
        """Set parameters to EIH camera via gRPC"""
        params = self.camera_params
        
        self.get_logger().info(
            f"Setting camera parameters:\n"
            f"  shutter_time: {params['shutter_time']}\n"
            f"  gain: {params['gain']}\n"
            f"  wb_r: {params['wb_redratio']}\n"
            f"  wb_g: {params['wb_greenratio']}\n"
            f"  wb_b: {params['wb_blueratio']}\n"
            f"  focus: {params['focus']}\n"
            f"  image_size: {params['image_size']}"
        )
        
        # Check if any setting is non-default
        any_setting = (
            params['shutter_time'] >= 0 or
            params['gain'] >= 0 or
            params['wb_redratio'] >= 0 or
            params['wb_greenratio'] >= 0 or
            params['wb_blueratio'] >= 0 or
            params['focus'] >= 0 or
            params['image_size']
        )
        
        if any_setting:
            result = self.client.set_capturing_settings(
                shutter_time=params['shutter_time'],
                gain=params['gain'],
                wb_redratio=params['wb_redratio'],
                wb_greenratio=params['wb_greenratio'],
                wb_blueratio=params['wb_blueratio'],
                focus=params['focus'],
                image_size=params['image_size']
            )
            
            if result.status == 0:
                self.get_logger().info("Applied capturing settings via EIH API")
            else:
                self.get_logger().error(f"Failed to set parameters: {result.error_message}")
    
    def _publish_image(self):
        """Publish image and camera info"""
        try:
            # Get image data
            t0 = time.time()
            result, image_data = self.client.get_image_data()
            t1 = time.time()
            
            if result.status != 0 or image_data is None:
                self.get_logger().warn(f"Failed to get image data: {result.error_message}")
                return
            
            if len(image_data) == 0:
                self.get_logger().warn("Received empty image data!")
                return
            
            # Publish compressed image
            header = Header()
            header.stamp = self.get_clock().now().to_msg()
            header.frame_id = self.frame_id
            
            comp_msg = CompressedImage()
            comp_msg.header = header
            comp_msg.format = "png"
            comp_msg.data = image_data
            self.compressed_pub.publish(comp_msg)
            
            # Decode image
            t2 = time.time()
            np_arr = np.frombuffer(image_data, np.uint8)
            img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
            t3 = time.time()
            
            self.get_logger().debug(
                f"Cost time: getImageData={((t1-t0)*1000):.2f} ms, "
                f"decode Image={((t3-t2)*1000):.2f} ms"
            )
            
            if img is not None and img.size > 0:
                # Publish raw image
                img_msg = self.bridge.cv2_to_imgmsg(img, encoding=self.image_encoding)
                img_msg.header = header
                self.image_pub.publish(img_msg)
                
                self.get_logger().debug("Published image data")
                
                # Publish camera info
                self._publish_camera_info(header)
            else:
                self.get_logger().warn("Failed to decode image!")
                
        except Exception as e:
            self.get_logger().error(f"Error in publish_image: {str(e)}")
    
    def _publish_camera_info(self, header: Header):
        """Publish camera info message"""
        try:
            # Get intrinsics
            result, intrinsics_list = self.client.get_intrinsics()
            if result.status != 0 or intrinsics_list is None:
                return
            
            # Get current focus and image configuration
            t0 = self.get_clock().now()
            result_focus, focus_data = self.client.get_focus()
            result_config, image_config = self.client.get_image_configuration()
            t1 = self.get_clock().now()
            
            duration_ms = (t1 - t0).nanoseconds / 1e6
            self.get_logger().debug(
                f"getFocus & getImageConfiguration cost: {duration_ms:.2f} ms"
            )
            
            if result_focus.status != 0 or focus_data is None:
                return
            if result_config.status != 0 or image_config is None:
                return
            
            current_focus = focus_data['current_value']
            self.get_logger().debug(f"focus {current_focus}")
            self.get_logger().debug(f"image width: {image_config['image_width']}")
            self.get_logger().debug(f"image height: {image_config['image_height']}")
            
            # Map focus value (valid range 2-7)
            mapped_focus = current_focus
            if mapped_focus < 2:
                mapped_focus = 2
            elif mapped_focus > 7:
                mapped_focus = 7
            
            # Find matching intrinsics
            for intr in intrinsics_list:
                if (intr['focus_value'] == mapped_focus and
                    intr['image_height'] == image_config['image_height']):
                    
                    cam_info = CameraInfo()
                    cam_info.header = header
                    cam_info.height = intr['image_height']
                    cam_info.width = intr['image_width']
                    cam_info.distortion_model = "plumb_bob"
                    
                    # Distortion coefficients
                    cam_info.d = [
                        float(intr['distortion_coefficients']['coefficient_00']),
                        float(intr['distortion_coefficients']['coefficient_10']),
                        float(intr['distortion_coefficients']['coefficient_20']),
                        float(intr['distortion_coefficients']['coefficient_30']),
                        float(intr['distortion_coefficients']['coefficient_40']),
                    ]
                    
                    # Camera matrix K
                    cm = intr['camera_matrix']
                    cam_info.k = [
                        float(cm['matrix_00']), float(cm['matrix_01']), float(cm['matrix_02']),
                        float(cm['matrix_10']), float(cm['matrix_11']), float(cm['matrix_12']),
                        float(cm['matrix_20']), float(cm['matrix_21']), float(cm['matrix_22']),
                    ]
                    
                    # Rectification matrix R (identity)
                    cam_info.r = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
                    
                    # Projection matrix P
                    cam_info.p = [
                        float(cm['matrix_00']), float(cm['matrix_01']), float(cm['matrix_02']), 0.0,
                        float(cm['matrix_10']), float(cm['matrix_11']), float(cm['matrix_12']), 0.0,
                        float(cm['matrix_20']), float(cm['matrix_21']), float(cm['matrix_22']), 0.0,
                    ]
                    
                    self.camera_info_pub.publish(cam_info)
                    break
                    
        except Exception as e:
            self.get_logger().error(f"Error in publish_camera_info: {str(e)}")


def main(args=None):
    rclpy.init(args=args)
    node = EIHClientPublisher()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
