from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument

def generate_launch_description():
    robot_ip_arg = DeclareLaunchArgument(
        'robot_ip',
        default_value='172.25.181.19',  # for test, will remove
        description='Server address for the robot EIH'
    )

    return LaunchDescription([
        robot_ip_arg,
        Node(
            package='eih_client',
            executable='eih_client_pub',
            parameters=[{'robot_ip': LaunchConfiguration('robot_ip')}],
        ),
    ])