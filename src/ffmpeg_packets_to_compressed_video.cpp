#include <rclcpp/rclcpp.hpp>
#include <foxglove_msgs/msg/compressed_video.hpp>
#include <ffmpeg_image_transport_msgs/msg/ffmpeg_packet.hpp>
#include <algorithm>
#include <string>
#include <vector>

using foxglove_msgs::msg::CompressedVideo;
using ffmpeg_image_transport_msgs::msg::FFMPEGPacket;


  class PacketsToCompressedVideo : public rclcpp::Node {
  public:
    PacketsToCompressedVideo() : Node("packets_to_compressed_video") {
    // Parameters
    frame_id_     = this->declare_parameter<std::string>("frame_id", "camera");
    input_topic_  = this->declare_parameter<std::string>("input_topic", "video/packets");
    output_topic_ = this->declare_parameter<std::string>("output_topic", "video/compressed");

      auto qos = rclcpp::SensorDataQoS();  
    pub_ = this->create_publisher<CompressedVideo>(output_topic_, qos);

    sub_ = this->create_subscription<FFMPEGPacket>(
      input_topic_, qos,
      std::bind(&PacketsToCompressedVideo::input_callback, this, std::placeholders::_1));

      RCLCPP_INFO(get_logger(), "Subscribing packets: %s", input_topic_.c_str());
      RCLCPP_INFO(get_logger(), "Publishing CompressedVideo: %s (h264)", output_topic_.c_str());
    }

  private:
  void input_callback(const FFMPEGPacket::SharedPtr msg) {
    const auto &in = msg->data;
    if (in.empty()) return;

    // Hardcode encoding to h264
    std::string encoding = "h264";

    CompressedVideo out;
    // Use timestamp from FFMPEGPacket header if available
    if (msg->header.stamp.sec != 0 || msg->header.stamp.nanosec != 0) {
      out.timestamp = msg->header.stamp;
    } else {
      auto now = this->get_clock()->now();
      out.timestamp.sec = now.seconds();
      out.timestamp.nanosec = now.nanoseconds() % 1000000000UL;
    }
    
    // Use frame_id from FFMPEGPacket header if available
    out.frame_id = msg->header.frame_id.empty() ? frame_id_ : msg->header.frame_id;
    out.format = encoding;
    out.data.reserve(in.size());
    out.data.assign(in.begin(), in.end());

    pub_->publish(std::move(out));
  }

  rclcpp::Publisher<CompressedVideo>::SharedPtr pub_;
  rclcpp::Subscription<FFMPEGPacket>::SharedPtr sub_;
  std::string frame_id_, input_topic_, output_topic_;
  };

  int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PacketsToCompressedVideo>());
    rclcpp::shutdown();
    return 0;
  }