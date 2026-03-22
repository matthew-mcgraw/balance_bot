#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <std_msgs/msg/float64.hpp>
#include <cmath>

class PID_Controller : public rclcpp::Node {
    public:
        PID_Controller() : rclcpp::Node("pid_controller_node"){
            this->declare_parameter<int>("kp", -1.0);
            this->declare_parameter<int>("ki", -1.0);
            this->declare_parameter<int>("kd", -1.0);

            kp_ = this->get_parameter("kp").as_double();
            ki_ = this->get_parameter("ki").as_double();
            kd_ = this->get_parameter("kd").as_double();
            


        }

        ~PID_Controller() {

        }

    private:
        double kp_{-1.0};
        double ki_{-1.0};
        double kd_{-1.0};

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