#include <EIHCameraApiClient.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <vector>

using namespace TmEIHCamera;

class EIHCameraNode : public rclcpp::Node {
 public:
  EIHCameraNode() : Node("eih_camera_node") {
    image_publisher_ =
        this->create_publisher<sensor_msgs::msg::Image>("eih_image", 10);
    std::string server_address = "172.25.181.19:15567";
    client_ = std::make_unique<EIHCameraApiClient>(server_address);

    client_->getImageConfiguration(result_, eih_image_.config);
    timer_ =
        this->create_wall_timer(std::chrono::milliseconds(33),
                                std::bind(&EIHCameraNode::publish_image, this));
  }

 private:
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<EIHCameraApiClient> client_;
  TmEIHConfig::Image eih_image_;
  GrpcResult result_;

  void publish_image() {
    client_->getImageData(result_, eih_image_.byte_data);  // to do : name style

    if (!eih_image_.byte_data.empty()) {
      cv::Mat img =
          cv::imdecode(cv::Mat(eih_image_.byte_data), cv::IMREAD_COLOR);

      if (!img.empty()) {
        auto ros_img_msg =
            cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", img)
                .toImageMsg();
        ros_img_msg->header.stamp = this->now();

        image_publisher_->publish(*ros_img_msg);
        RCLCPP_INFO(this->get_logger(), "Received image data");
      }
    } else {
      RCLCPP_WARN(this->get_logger(), "Received empty image data!");
    }
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<EIHCameraNode>());
  rclcpp::shutdown();
  return 0;
}
