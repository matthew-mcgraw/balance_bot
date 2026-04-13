#include <rclcpp/rclcpp.hpp>       //ROS2 node API
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <std_msgs/msg/float64.hpp>
#include <cmath>

class PID_Controller : public rclcpp::Node
{
public:
    PID_Controller() : rclcpp::Node("pid_controller_node")
    {
        this->declare_parameter<double>("kp", -1.0);
        this->declare_parameter<double>("ki", -1.0);
        this->declare_parameter<double>("kd", -1.0);
        this->declare_parameter<double>("setpoint", -1.0);
        this->declare_parameter<double>("fall_detection_threshold", 0.5);

        kp_ = this->get_parameter("kp").as_double();
        ki_ = this->get_parameter("ki").as_double();
        kd_ = this->get_parameter("kd").as_double();
        setpoint_ = this->get_parameter("setpoint").as_double();
        fall_detection_threshold_ = this->get_parameter("fall_detection_threshold").as_double();

        imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/imu/data_raw", 50,
            std::bind(&PID_Controller::on_imu, this, std::placeholders::_1));

        pitch_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/pitch", 10,
            std::bind(&PID_Controller::on_pitch, this, std::placeholders::_1));

        force_pub_ = this->create_publisher<std_msgs::msg::Float64>("/force_setpoint", 10);
    }

    ~PID_Controller()
    {
    }

private:
    double kp_{-1.0};                      // Proportional gain, adjust based on your robot's response (start with a high value and decrease if the system oscillates)
    double ki_{-1.0};                      // Integral gain, can be set to 0 for a start, increase if you see steady-state error (robot not perfectly balanced), but be careful as too high value can cause instability
    double kd_{-1.0};                      // Derivative gain, can be set to 0 for a start, increase if you see excessive oscillation, but be careful as too high value can cause instability
    double setpoint_{0.0};                 // Desired pitch angle (0 degrees for upright balance)
    double latest_pitch_rate_{0.0};        // Latest pitch rate from the IMU (gyro x), used for the derivative term in PID
    double fall_detection_threshold_{0.5}; // in radians, if the pitch angle exceeds this threshold, we consider the robot has fallen and stop applying corrective force

    // add to private members
    double integral_{0.0};
    rclcpp::Time last_time_;
    bool have_last_time_{false};

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;    // subscribe to raw IMU data to get the latest pitch rate (gyro x)
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr pitch_sub_; // subscribe to the estimated pitch angle to calculate the PID output
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr force_pub_;    // publish cmd_vel with linear.x set to the PWM output for the motor controller node to consume

    void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg)
    {
        const double gx = msg->angular_velocity.x;
        latest_pitch_rate_ = gx;
    }

    void on_pitch(const std_msgs::msg::Float64::SharedPtr msg)
    {
        const double pitch = msg->data;

        if (std::abs(pitch) > fall_detection_threshold_)
        {
            integral_ = 0.0; // reset integral on fall
            have_last_time_ = false;
            std_msgs::msg::Float64 zero_msg;
            zero_msg.data = 0.0;
            force_pub_->publish(zero_msg);
            return;
        }

        const auto now = this->now();
        if (!have_last_time_)
        {
            last_time_ = now;
            have_last_time_ = true;
            return;
        }
        const double dt = (now - last_time_).seconds();
        last_time_ = now;
        if (dt <= 0.0 || dt > 0.1)
            return;

        const double error = setpoint_ - pitch;
        integral_ += error * dt;
        integral_ = std::clamp(integral_, -2.0, 2.0); // anti-windup

        const double output_ = (kp_ * error) + (ki_ * integral_) + (kd_ * latest_pitch_rate_);
        std_msgs::msg::Float64 force_cmd_msg;
        force_cmd_msg.data = output_;
        force_pub_->publish(force_cmd_msg);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PID_Controller>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}