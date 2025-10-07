#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

class ImageSubscriberNode : public rclcpp::Node {
 public:
  ImageSubscriberNode() : Node("image_subscriber_node") {
    image_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/eih_camera/image_raw", rclcpp::QoS(100),
        std::bind(&ImageSubscriberNode::image_callback, this,
                  std::placeholders::_1));
  }

 private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
    try {
      cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;
      cv::Mat resized_image;
      cv::resize(image, resized_image, cv::Size(640, 480));

      cv::imshow("Subscribed Image", resized_image);
      cv::waitKey(1);
    } catch (const cv_bridge::Exception &e) {
      RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
    }
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscription_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ImageSubscriberNode>());
  rclcpp::shutdown();
  return 0;
}