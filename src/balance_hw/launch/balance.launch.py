from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():

    config = os.path.join(
        get_package_share_directory("balance_hw"), "config", "robot.yaml"
    )

    return LaunchDescription(
        [
            Node(
                package="balance_hw",
                executable="imu_node",
                name="imu_node",
                parameters=[config],
            ),
            Node(
                package="balance_hw",
                executable="imu_estimator_node",
                name="imu_estimator_node",
                parameters=[config],
            ),
            Node(
                package="balance_hw",
                executable="pid_controller_node",
                name="pid_controller_node",
                parameters=[config],
            ),
            Node(
                package="balance_hw",
                executable="motor_encoder_node",
                name="motor_encoder_node",
                parameters=[config],
            ),
            Node(
                package="balance_hw",
                executable="velocity_controller_node",
                name="velocity_controller_node",
                parameters=[config],
            ),
            Node(
                package="balance_hw",
                executable="motor_enable_node",
                name="motor_enable_node",
                parameters=[config],
            ),
        ]
    )
