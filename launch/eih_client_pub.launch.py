from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.actions import DeclareLaunchArgument
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    robot_ip_arg = DeclareLaunchArgument(
        'robot_ip',
        default_value='172.25.181.18',  # for test, will remove
        description='Server address for the robot EIH'
    )
    params_file_arg = DeclareLaunchArgument(
        'params_file',
        default_value=PathJoinSubstitution(
            [FindPackageShare('eih_client'), 'config', 'eih_params.yaml']
        ),
        description='Path to eih params yaml'
    )

    return LaunchDescription([
        robot_ip_arg,
        params_file_arg,
        Node(
            package='eih_client',
            executable='eih_client_pub',
            output='screen',
            parameters=[
                {'robot_ip': LaunchConfiguration('robot_ip')},
                [LaunchConfiguration('params_file')]
            ],
        ),
    ])