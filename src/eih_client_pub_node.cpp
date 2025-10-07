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

    // init eih params
    get_eih_params();
    set_params_to_eih_camera();

    // register parameter change callback (will be invoked on ros2 param set)
    params_cb_handle_ = this->add_on_set_parameters_callback(
        std::bind(&EIHClientPublisher::on_params_change, this, std::placeholders::_1));
    
    image_pub_ = this->create_publisher<sensor_msgs::msg::Image>(
        "/eih_camera/image_raw", rclcpp::QoS(100));
    compressed_pub_ = this->create_publisher<sensor_msgs::msg::CompressedImage>(
        "/eih_camera/image_raw/compressed", rclcpp::QoS(100));

    camera_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(
        "/eih_camera/camera_info", rclcpp::QoS(100));

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(33),
        std::bind(&EIHClientPublisher::publish_image, this));
  }

 private:
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr params_cb_handle_;
  std::string camera_addr_;
  std::string frame_id_;
  std::string image_encoding_;

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr compressed_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::unique_ptr<EIHCameraApiClient> client_;
  tm_eih_config::Image eih_img_;
  tm_eih_config::SetCapturingSettingsRequest req_;
  GrpcResult grpc_result_;

  rcl_interfaces::msg::SetParametersResult on_params_change(const std::vector<rclcpp::Parameter> &params) {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    // re-apply relevant camera params when any parameter changes
    assign_params(params);
    set_params_to_eih_camera();
    return result;
  }

  void get_eih_params() {
    auto paras_client = std::make_shared<rclcpp::SyncParametersClient>(this);
    auto params = paras_client->get_parameters(
      {
        "shutter_time", "gain", "wb_redratio", "wb_greenratio", "wb_blueratio", "focus", "image_size"
      }
    );
    assign_params(params);
  }

  void assign_params(const std::vector<rclcpp::Parameter> & params) {
    for (const auto &p : params) {
      const auto &name = p.get_name();
      if (name == "shutter_time") req_.shutter_time = p.as_int();
      else if (name == "gain") req_.gain = p.as_int();
      else if (name == "wb_redratio") req_.wb_redratio = p.as_int();
      else if (name == "wb_greenratio") req_.wb_greenratio = p.as_int();
      else if (name == "wb_blueratio") req_.wb_blueratio = p.as_int();
      else if (name == "focus") req_.focus = p.as_int();
      else if (name == "image_size") req_.image_size = p.as_string();
      else {
        RCLCPP_WARN(this->get_logger(), "Invalid parameter name: %s", name.c_str());
      }
    }
  }

  void set_params_to_eih_camera() {
    RCLCPP_INFO(this->get_logger(),
      "shutter_time: %d\n"
      "gain: %d\n"
      "wb_r: %d\n"
      "wb_g: %d\n"
      "wb_b: %d\n"
      "focus: %d\n"
      "image_size: %s",
      req_.shutter_time, req_.gain, req_.wb_redratio, req_.wb_greenratio, req_.wb_blueratio, req_.focus, req_.image_size.c_str());

    bool any_setting = (req_.shutter_time >= 0) || (req_.gain >= 0) || (req_.wb_redratio >= 0) ||
                     (req_.wb_greenratio >= 0) || (req_.wb_blueratio >= 0) || (req_.focus >= 0) ||
                     (!req_.image_size.empty());
    RCLCPP_DEBUG(this->get_logger(), "any_setting: %d", any_setting);

    if (any_setting) {
      client_->setCapturingSettings(grpc_result_, req_);
      RCLCPP_INFO(this->get_logger(), "Applied capturing settings via EIH api setCapturingSettings");
    }
  }


  // Image
  void publish_image() {

    auto t0 = std::chrono::steady_clock::now();
    client_->getImageData(grpc_result_, eih_img_.data);
    auto t1 = std::chrono::steady_clock::now();

    auto img_raw = eih_img_.data.encode_string;

    if (!img_raw.empty()) {

      sensor_msgs::msg::CompressedImage comp_msg;
      comp_msg.header.stamp = this->now();
      comp_msg.header.frame_id = frame_id_;
      comp_msg.format = "png";
      comp_msg.data.assign(img_raw.begin(), img_raw.end());
      compressed_pub_->publish(comp_msg);

      auto t2 = std::chrono::steady_clock::now();
      cv::Mat img = cv::imdecode(cv::Mat(img_raw), cv::IMREAD_COLOR);
      auto t3 = std::chrono::steady_clock::now();

      RCLCPP_DEBUG(this->get_logger(),
        "Cost time: getImageData=%.2f ms, decode Image=%.2f ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() * 1.0,
        std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count() * 1.0
      );

      if (!img.empty()) {
        auto img_msg =
            cv_bridge::CvImage(std_msgs::msg::Header(), image_encoding_, img)
                .toImageMsg();
        img_msg->header.stamp = this->now();
        img_msg->header.frame_id = frame_id_;
        image_pub_->publish(*img_msg);
        RCLCPP_DEBUG(this->get_logger(), "Received image data");

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

    auto start = this->now();
    client_->getFocus(grpc_result_, focus);
    client_->getImageConfiguration(grpc_result_, eih_img_.config);
    auto end = this->now();
    auto duration_ms = (end - start).seconds() * 1000.0;
    RCLCPP_DEBUG(this->get_logger(), "getFocus & getImageConfiguration cost: %.2f ms", duration_ms);
    
    RCLCPP_DEBUG(this->get_logger(), "focus %d", focus.current_value);
    RCLCPP_DEBUG(this->get_logger(), "image width: %d", eih_img_.config.image_width);
    RCLCPP_DEBUG(this->get_logger(), "image height: %d", eih_img_.config.image_height);

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
