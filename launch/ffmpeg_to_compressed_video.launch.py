#!/usr/bin/env python3

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    # Declare launch arguments
    format_arg = DeclareLaunchArgument(
        "format", default_value="h264", description="Video format: h264, h265, vp9, av1"
    )

    frame_id_arg = DeclareLaunchArgument(
        "frame_id",
        default_value="camera",
        description="Frame ID for the video messages",
    )

    input_topic_arg = DeclareLaunchArgument(
        "input_topic",
        default_value="video/packets",
        description="Input topic for video packets (UInt8MultiArray)",
    )

    output_topic_arg = DeclareLaunchArgument(
        "output_topic",
        default_value="video/compressed",
        description="Output topic for compressed video messages",
    )

    # Node
    packets_node = Node(
        package="ffmpeg_image_transport",
        executable="ffmpeg_packets_to_compressed_video_node",
        name="packets_to_compressed_video",
        parameters=[
            {"format": LaunchConfiguration("format")},
            {"frame_id": LaunchConfiguration("frame_id")},
            {"input_topic": LaunchConfiguration("input_topic")},
            {"output_topic": LaunchConfiguration("output_topic")},
        ],
        output="screen",
        emulate_tty=True,
    )

    return LaunchDescription(
        [
            format_arg,
            frame_id_arg,
            input_topic_arg,
            output_topic_arg,
            packets_node,
        ]
    )
