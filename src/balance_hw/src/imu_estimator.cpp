#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <std_msgs/msg/float64.hpp>
#include <cmath>

class IMU_Estimator : public rclcpp::Node {
    public:
        IMU_Estimator() : rclcpp::Node("imu_estimator_node"){


            imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
                "/imu/data_raw", 50,
                std::bind(&IMU_Estimator::on_imu, this, std::placeholders::_1)
            );
            pitch_pub_ = this->create_publisher<std_msgs::msg::Float64>("/pitch", 10);




        }

        ~IMU_Estimator() {

        }

    private:
        bool have_last_time_{false};
        rclcpp::Time last_time_;
        double pitch_rad_{0.0};
        double alpha_{0.98}; // closer to 1, use gyro estimation more, closer to 0, use accel estimation more


        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
        rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pitch_pub_;

        void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg){
            const auto msg_time = rclcpp::Time(msg->header.stamp);

            

            if(!have_last_time_){
                last_time_ = msg_time;
                have_last_time_ = true;
                return;
            }
            if(msg_time.nanoseconds() ==  0) return;

            const double dt = (msg_time - last_time_).seconds(); //calculate time difference to integrate gyro measurement
            last_time_ = msg_time;

            // do not want to use negative time, or use a large delta,
            // balancing inverted pendulum smoothly requires very small theta, which also means very small time difference
            if(dt <= 0.0 || dt> 0.1) return; 

            const double ax = msg->linear_acceleration.x;
            const double ay = msg->linear_acceleration.y;
            const double az = msg->linear_acceleration.z;

            const double gx = msg->angular_velocity.x;

            const double pitch_acc = std::atan2(-ay, std::sqrt(ax*ax + az*az));
            const double pitch_gyro = pitch_rad_ + gx*dt;

            pitch_rad_ = alpha_ * pitch_gyro + (1.0 - alpha_) * pitch_acc;
            
            std_msgs::msg::Float64 out;
            out.data = pitch_rad_;
            pitch_pub_->publish(out);

        }



};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<IMU_Estimator>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}