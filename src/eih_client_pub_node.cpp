#include <cv_bridge/cv_bridge.h>
#include <eih_camera_api_client.h>

#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <vector>

using namespace TmEIHCamera;

class EIHClientPublisher : public rclcpp::Node {
 public:
  EIHClientPublisher() : Node("eih_client_publisher") {
    this->declare_parameter<std::string>("robot_ip", "");
    this->declare_parameter<std::string>("frame_id", "eih_camera");
    this->declare_parameter<std::string>("image_encoding", "bgr8");

    std::string robot_ip_ = this->get_parameter("robot_ip").as_string();
    camera_addr_ = robot_ip_ + ":15567";
    frame_id_ = this->get_parameter("frame_id").as_string();
    image_encoding_ = this->get_parameter("image_encoding").as_string();
    client_ = std::make_unique<EIHCameraApiClient>(camera_addr_);

    image_pub_ = this->create_publisher<sensor_msgs::msg::Image>(
        "/eih_camera/image_raw", rclcpp::SensorDataQoS());
    // camera_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(
    //     "/eih_camera/camera_info", rclcpp::SensorDataQoS());

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(33),
        std::bind(&EIHClientPublisher::publish_image, this));
  }

 private:
  std::string camera_addr_;
  std::string frame_id_;
  std::string image_encoding_;

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
  // rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr
  // camera_info_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<EIHCameraApiClient> client_;
  TmEIHConfig::Image eih_img_;
  GrpcResult grpc_result_;

  // Image
  void publish_image() {
    // client_->getImageData(grpc_result_, eih_image_.data.encode_string);
    client_->getImageData(grpc_result_, eih_img_.data);
    auto img_raw = eih_img_.data.encode_string;

    if (!img_raw.empty()) {
      cv::Mat img = cv::imdecode(cv::Mat(img_raw), cv::IMREAD_COLOR);

      if (!img.empty()) {
        auto img_msg =
            cv_bridge::CvImage(std_msgs::msg::Header(), image_encoding_, img)
                .toImageMsg();
        img_msg->header.stamp = this->now();
        img_msg->header.frame_id = frame_id_;
        image_pub_->publish(*img_msg);
        RCLCPP_INFO(this->get_logger(), "Received image data");
      }
    } else {
      RCLCPP_WARN(this->get_logger(), "Received empty image data!");
    }
  }

  // CameraInfo
  // void publish_info() {
  //     sensor_msgs::msg::CameraInfo cam_info;
  //     cam_info.header = ros_img_msg->header;
  //
  //     cam_info.width = ...;
  //     cam_info.height = ...;
  //     cam_info.k = {...};
  //     cam_info.d = {...};
  // }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<EIHClientPublisher>());
  rclcpp::shutdown();
  return 0;
}
