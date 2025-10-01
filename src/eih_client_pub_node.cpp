#include <cv_bridge/cv_bridge.h>
#include <eih_camera_api_client.h>

#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <vector>
#include <rcl_interfaces/msg/set_parameters_result.hpp>

using namespace TmEIHCamera;

class EIHClientPublisher : public rclcpp::Node {
 public:
  EIHClientPublisher() : Node("eih_client_publisher") {
    this->declare_parameter<std::string>("robot_ip", "");
    this->declare_parameter<std::string>("frame_id", "eih_camera");
    this->declare_parameter<std::string>("image_encoding", "bgr8");

    this->declare_parameter<int>("shutter_time", -1);
    this->declare_parameter<int>("gain", -1);
    this->declare_parameter<int>("wb_redratio", -1);
    this->declare_parameter<int>("wb_greenratio", -1);
    this->declare_parameter<int>("wb_blueratio", -1);
    this->declare_parameter<int>("focus", -1);
    this->declare_parameter<std::string>("image_size", "");

    std::string robot_ip_ = this->get_parameter("robot_ip").as_string();
    camera_addr_ = robot_ip_ + ":15567";
    frame_id_ = this->get_parameter("frame_id").as_string();
    image_encoding_ = this->get_parameter("image_encoding").as_string();
    client_ = std::make_unique<EIHCameraApiClient>(camera_addr_);

    // init params
    set_camera_params();

    // register parameter change callback (will be invoked on ros2 param set)
    param_cb_handle_ = this->add_on_set_parameters_callback(
        std::bind(&EIHClientPublisher::on_params_change, this, std::placeholders::_1));
    
    image_pub_ = this->create_publisher<sensor_msgs::msg::Image>(
        "/eih_camera/image_raw", rclcpp::SensorDataQoS());
    camera_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(
        "/eih_camera/camera_info", rclcpp::SensorDataQoS());

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(33),
        std::bind(&EIHClientPublisher::publish_image, this));
  }

 private:
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_handle_;

  std::string camera_addr_;
  std::string frame_id_;
  std::string image_encoding_;

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<EIHCameraApiClient> client_;
  tm_eih_config::Image eih_img_;
  GrpcResult grpc_result_;

  rcl_interfaces::msg::SetParametersResult on_params_change(const std::vector<rclcpp::Parameter> &params) {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    // re-apply relevant camera params when any parameter changes
    set_camera_params();
    return result;
  }

  void set_camera_params() {
    int shutter_time = this->get_parameter("shutter_time").as_int();
    int gain = this->get_parameter("gain").as_int();
    int wb_r = this->get_parameter("wb_redratio").as_int();
    int wb_g = this->get_parameter("wb_greenratio").as_int();
    int wb_b = this->get_parameter("wb_blueratio").as_int();
    int focus = this->get_parameter("focus").as_int();
    std::string image_size = this->get_parameter("image_size").as_string();

    std::cout << "shutter_time: " << shutter_time << std::endl;
    std::cout << "gain: " << gain << std::endl;
    std::cout << "wb_redratio: " << wb_r << std::endl;
    std::cout << "wb_greenratio: " << wb_g << std::endl;
    std::cout << "wb_b: " << wb_b << std::endl;
    std::cout << "focus: " << focus << std::endl;
    std::cout << "image_size: " << image_size << std::endl;

    bool any_setting = (shutter_time > 0) || (gain >= 0) || (wb_r >= 0) ||
                     (wb_g >= 0) || (wb_b >= 0) || (focus >= 0) ||
                     (!image_size.empty());
    std::cout << "any_setting: " << any_setting << std::endl;

    if (any_setting) {
      tm_eih_config::SetCapturingSettingsRequest req;
      if (shutter_time > 0) req.shutter_time = shutter_time;
      if (gain >= 0) req.gain = gain;
      if (wb_r >= 0) req.wb_redratio = wb_r;
      if (wb_g >= 0) req.wb_greenratio = wb_g;
      if (wb_b >= 0) req.wb_blueratio = wb_b;
      if (focus >= 0) req.focus = focus;
      if (!image_size.empty()) req.image_size = image_size;
      client_->setCapturingSettings(grpc_result_, req);
      RCLCPP_INFO(this->get_logger(), "Applied capturing settings via setCapturingSettings");
    }
  }

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
    std::vector<tm_eih_config::Intrinsics> intrinsics_vec;
    if (!client_->getIntrinsics(grpc_result_, intrinsics_vec)) return;
    tm_eih_config::CaptureSettingValue focus;
    client_->getFocus(grpc_result_, focus);
    client_->getImageConfiguration(grpc_result_, eih_img_.config);
    std::cout << "focus: " << focus.current_value << std::endl;
    std::cout << "image width: " << eih_img_.config.image_width << std::endl;
    std::cout << "image height: " << eih_img_.config.image_height << std::endl;

    sensor_msgs::msg::CameraInfo cam_info;
    int mapped_focus = focus.current_value;
    if (mapped_focus < 2)
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
