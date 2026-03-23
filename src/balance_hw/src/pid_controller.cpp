#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <std_msgs/msg/float64.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cmath>

class PID_Controller : public rclcpp::Node {
    public:
        PID_Controller() : rclcpp::Node("pid_controller_node"){
            this->declare_parameter<double>("kp", -1.0);
            this->declare_parameter<double>("ki", -1.0);
            this->declare_parameter<double>("kd", -1.0);
            this->declare_parameter<double>("setpoint", -1.0);
            this->declare_parameter<double>("motor_stall_torque", -1.0);
            this->declare_parameter<double>("wheel_radius", -1.0);

            kp_ = this->get_parameter("kp").as_double();
            ki_ = this->get_parameter("ki").as_double();
            kd_ = this->get_parameter("kd").as_double();
            setpoint_ = this->get_parameter("setpoint").as_double();
            motor_stall_torque_ = this->get_parameter("motor_stall_torque").as_double();
            wheel_radius_ = this->get_parameter("wheel_radius").as_double();
            

            imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
                "/imu/data_raw", 50,
                std::bind(&PID_Controller::on_imu, this, std::placeholders::_1)
            );

            pitch_sub_ = this->create_subscription<std_msgs::msg::Float64>(
                "/pitch", 10,
                std::bind(&PID_Controller::on_pitch, this, std::placeholders::_1)
            );

            vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        }

        ~PID_Controller() {

        }

    private:
        double kp_{-1.0}; // Proportional gain, adjust based on your robot's response (start with a high value and decrease if the system oscillates)
        double ki_{-1.0}; // Integral gain, can be set to 0 for a start, increase if you see steady-state error (robot not perfectly balanced), but be careful as too high value can cause instability
        double kd_{-1.0}; // Derivative gain, can be set to 0 for a start, increase if you see excessive oscillation, but be careful as too high value can cause instability
        double setpoint_{0.0}; // Desired pitch angle (0 degrees for upright balance)
        double latest_pitch_rate_{0.0};
        double output_{0.0};
        int pwm_output_{0};
        double motor_stall_torque_{0.0}; // in N-m, adjust based on your motor's specifications
        double wheel_radius_{0.0}; // in meters, adjust based on your robot's wheel size

        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_; //subscribe to raw IMU data to get the latest pitch rate (gyro x)
        rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr pitch_sub_; //subscribe to the estimated pitch angle to calculate the PID output
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_; //publish cmd_vel with linear.x set to the PWM output for the motor controller node to consume

        void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg){
            const double gx = msg->angular_velocity.x;
            latest_pitch_rate_ = gx;
        }

        void on_pitch(const std_msgs::msg::Float64::SharedPtr msg){
            const double pitch = msg->data;
            const double error = 0.0 - pitch; // setpoint is 0.0, we want to balance at 0 degree
            
            output_ = (kp_ * error) + (ki_ * 0.0) + (kd_ * latest_pitch_rate_);

            const double pwm_output_float = (output_ * wheel_radius_) / motor_stall_torque_ ; // Scale to PWM range (0-1000)
            pwm_output_ = static_cast<int>(std::round(pwm_output_float));

            geometry_msgs::msg::Twist cmd_vel_msg;
            cmd_vel_msg.linear.x = pwm_output_; // Normalized torque pwm command, not m/s
            vel_pub_->publish(cmd_vel_msg);



        }
          



};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PID_Controller>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}