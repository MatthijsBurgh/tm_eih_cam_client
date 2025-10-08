from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.actions import DeclareLaunchArgument
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    robot_ip_arg = DeclareLaunchArgument(
        'robot_ip',
        default_value='198.168.10.20',  # your tm robot ip
        description='Server address for the robot EIH camera'
    )
    params_file_arg = DeclareLaunchArgument(
        'params_file',
        default_value=PathJoinSubstitution(
            [FindPackageShare('tm_eih_cam_client'), 'config', 'eih_params.yaml']
        ),
        description='Path to eih params yaml'
    )

    return LaunchDescription([
        robot_ip_arg,
        params_file_arg,
        Node(
            package='tm_eih_cam_client',
            executable='eih_client_pub',
            output='screen',
            parameters=[
                {'robot_ip': LaunchConfiguration('robot_ip')},
                [LaunchConfiguration('params_file')]
            ],
            arguments=['--ros-args', '--log-level', 'debug']
        ),
    ])