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
    camera_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(
        "/eih_camera/camera_info", rclcpp::SensorDataQoS());

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(33),
        std::bind(&EIHClientPublisher::publish_image, this));
  }

 private:
  std::string camera_addr_;
  std::string frame_id_;
  std::string image_encoding_;

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<EIHCameraApiClient> client_;
  TmEIHConfig::Image eih_img_;
  GrpcResult grpc_result_;

  // Image
  void publish_image() {
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
        // RCLCPP_INFO(this->get_logger(), "ros info :test tag ");

        publish_camera_info(img_msg->header);
      }
    } else {
      RCLCPP_WARN(this->get_logger(), "Received empty image data!");
    }
  }

  // CameraInfo
  void publish_camera_info(const std_msgs::msg::Header& header) {
    std::vector<TmEIHConfig::Intrinsics> intrinsics_vec;
    if (!client_->getIntrinsics(grpc_result_, intrinsics_vec)) return;
    TmEIHConfig::CaptureSettingValue focus;
    client_->getFocus(grpc_result_, focus);
    client_->getImageConfiguration(grpc_result_, eih_img_.config);
    std::cout << "focus: " << focus.current_value << std::endl;
    std::cout << "image height: " << eih_img_.config.image_height << std::endl;

    sensor_msgs::msg::CameraInfo cam_info;
    int mapped_focus = focus.current_value;
    if (mapped_focus <= 1)
      mapped_focus = 2;
    else if (mapped_focus > 7)
      mapped_focus = 7;

    for (const auto& intr : intrinsics_vec) {
      if (intr.focus_value == mapped_focus &&
          intr.image_height == eih_img_.config.image_height) {
        cam_info.header = header;
        cam_info.height = intr.image_height;
        cam_info.width = intr.image_width;
        cam_info.distortion_model = "plumb_bob";
        cam_info.d = {
            static_cast<double>(intr.distortion_coefficients.coefficient_00),
            static_cast<double>(intr.distortion_coefficients.coefficient_10),
            static_cast<double>(intr.distortion_coefficients.coefficient_20),
            static_cast<double>(intr.distortion_coefficients.coefficient_30),
            static_cast<double>(intr.distortion_coefficients.coefficient_40)};
        cam_info.k = {static_cast<double>(intr.camera_matrix.matrix_00),
                      static_cast<double>(intr.camera_matrix.matrix_01),
                      static_cast<double>(intr.camera_matrix.matrix_02),
                      static_cast<double>(intr.camera_matrix.matrix_10),
                      static_cast<double>(intr.camera_matrix.matrix_11),
                      static_cast<double>(intr.camera_matrix.matrix_12),
                      static_cast<double>(intr.camera_matrix.matrix_20),
                      static_cast<double>(intr.camera_matrix.matrix_21),
                      static_cast<double>(intr.camera_matrix.matrix_22)};
        cam_info.r = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        cam_info.p = {static_cast<double>(intr.camera_matrix.matrix_00),
                      static_cast<double>(intr.camera_matrix.matrix_01),
                      static_cast<double>(intr.camera_matrix.matrix_02),
                      0,
                      static_cast<double>(intr.camera_matrix.matrix_10),
                      static_cast<double>(intr.camera_matrix.matrix_11),
                      static_cast<double>(intr.camera_matrix.matrix_12),
                      0,
                      static_cast<double>(intr.camera_matrix.matrix_20),
                      static_cast<double>(intr.camera_matrix.matrix_21),
                      static_cast<double>(intr.camera_matrix.matrix_22),
                      0};
        camera_info_pub_->publish(cam_info);
        break;
      }
    }
  }
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<EIHClientPublisher>());
  rclcpp::shutdown();
  return 0;
}
