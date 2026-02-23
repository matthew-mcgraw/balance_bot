#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <cstdint> // uint8_t, int16_t
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <chrono> //create timer/wallclock
#include <std_msgs/msg/float64.hpp>
#include <stdexcept> 
#include <cmath>

class IMU_Estimator : public rclcpp::Node {
    public:
        IMU_Estimator() : rclcpp::Node("imu_estimator_node"){


            imu_sub_ = this->create_subscriber<sensor_msgs::msg::Imu>("/imu/data_raw", 10);



            timer_ = this->create_wall_timer(
                std::chrono::milliseconds(5), // 200 Hz

                [this]() {

                    pitch_acc = std::atan2(-ax, std::sqrt(ay*ay + az*az));
                    

                }
            );


        }

        ~IMU_Node() {

        }

    private:
        const double g{9.807};
        const double deg2rad{3.14159/180.0}; 
        bool have_last_time_{false};
        rclcpp::Time last_time_;


        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
        rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pitch_pub_;
        rclcpp::TimerBase::SharedPtr timer_;



};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<IMU_Estimator>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}