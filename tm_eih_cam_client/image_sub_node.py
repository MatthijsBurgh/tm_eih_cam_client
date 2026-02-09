#!/usr/bin/env python3
"""Image Subscriber Node

Simple node to subscribe and display images from the EIH camera.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2


class ImageSubscriber(Node):
    """Simple image subscriber for testing"""
    
    def __init__(self):
        super().__init__('image_subscriber')
        
        self.subscription = self.create_subscription(
            Image,
            '/eih_camera/image_raw',
            self.image_callback,
            10
        )
        
        self.bridge = CvBridge()
        self.get_logger().info("Image subscriber initialized")
    
    def image_callback(self, msg):
        """Callback for image messages"""
        try:
            # Convert ROS Image to OpenCV image
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
            
            # Display image
            cv2.imshow('EIH Camera', cv_image)
            cv2.waitKey(1)
            
            self.get_logger().debug(
                f"Received image: {msg.width}x{msg.height}, "
                f"encoding: {msg.encoding}"
            )
        except Exception as e:
            self.get_logger().error(f"Error processing image: {str(e)}")


def main(args=None):
    rclpy.init(args=args)
    node = ImageSubscriber()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        cv2.destroyAllWindows()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
