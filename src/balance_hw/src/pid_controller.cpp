#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <std_msgs/msg/float64.hpp>
#include <cmath>

// Balance PID: reads pitch, outputs a desired wheel velocity (rad/s).
// Positive output = wheels spin forward = robot corrects backward tilt.
// The velocity controller then tracks this setpoint using encoder feedback.

class PID_Controller : public rclcpp::Node
{
public:
    PID_Controller() : rclcpp::Node("pid_controller_node")
    {
        this->declare_parameter<double>("kp", 30.0);
        this->declare_parameter<double>("ki", 0.0);
        this->declare_parameter<double>("kd", 2.0);
        this->declare_parameter<double>("setpoint", 0.0);
        this->declare_parameter<double>("fall_detection_threshold", 0.45);
        this->declare_parameter<double>("max_velocity_cmd", 15.0);

        kp_ = this->get_parameter("kp").as_double();
        ki_ = this->get_parameter("ki").as_double();
        kd_ = this->get_parameter("kd").as_double();
        setpoint_ = this->get_parameter("setpoint").as_double();
        fall_detection_threshold_ = this->get_parameter("fall_detection_threshold").as_double();
        max_velocity_cmd_ = this->get_parameter("max_velocity_cmd").as_double();

        imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/imu/data_raw", 50,
            std::bind(&PID_Controller::on_imu, this, std::placeholders::_1));

        pitch_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/pitch", 10,
            std::bind(&PID_Controller::on_pitch, this, std::placeholders::_1));

        // Output: desired wheel velocity in rad/s
        velocity_setpoint_pub_ = this->create_publisher<std_msgs::msg::Float64>("/velocity_setpoint", 10);

        RCLCPP_INFO(get_logger(),
                    "PID controller initialized: kp=%.2f ki=%.2f kd=%.2f setpoint=%.2f "
                    "fall_thresh=%.2f max_vel=%.1f",
                    kp_, ki_, kd_, setpoint_, fall_detection_threshold_, max_velocity_cmd_);
    }

private:
    double kp_, ki_, kd_;
    double setpoint_{0.0};
    double latest_pitch_rate_{0.0};
    double fall_detection_threshold_{0.45};
    double max_velocity_cmd_{15.0};
    bool fallen_{false};

    double integral_{0.0};
    rclcpp::Time last_time_;
    bool have_last_time_{false};

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr pitch_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr velocity_setpoint_pub_;

    void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg)
    {
        latest_pitch_rate_ = msg->angular_velocity.x;
    }

    void on_pitch(const std_msgs::msg::Float64::SharedPtr msg)
    {
        const double pitch = msg->data;

        // Fall detection with hysteresis latch
        if (fallen_) {
            if (std::abs(pitch) < fall_detection_threshold_ * 0.5) {
                fallen_ = false;
                integral_ = 0.0;
                have_last_time_ = false;
                RCLCPP_INFO(get_logger(), "Robot recovered, resuming control");
            } else {
                std_msgs::msg::Float64 zero_msg;
                zero_msg.data = 0.0;
                velocity_setpoint_pub_->publish(zero_msg);
                return;
            }
        }

        if (std::abs(pitch) > fall_detection_threshold_) {
            fallen_ = true;
            integral_ = 0.0;
            have_last_time_ = false;
            std_msgs::msg::Float64 zero_msg;
            zero_msg.data = 0.0;
            velocity_setpoint_pub_->publish(zero_msg);
            RCLCPP_WARN(get_logger(), "Fall detected! pitch=%.3f rad (%.1f deg), motors off",
                        pitch, pitch * 180.0 / 3.14159);
            return;
        }

        const auto now = this->now();
        if (!have_last_time_) {
            last_time_ = now;
            have_last_time_ = true;
            return;
        }
        const double dt = (now - last_time_).seconds();
        last_time_ = now;
        if (dt <= 0.0 || dt > 0.1) return;

        const double error = setpoint_ - pitch;

        integral_ += error * dt;
        integral_ = std::clamp(integral_, -2.0, 2.0);

        // D-term: negate gyro rate for proper damping
        const double derivative = -latest_pitch_rate_;

        double output = (kp_ * error) + (ki_ * integral_) + (kd_ * derivative);

        // Clamp to max velocity the motors can achieve
        output = std::clamp(output, -max_velocity_cmd_, max_velocity_cmd_);

        std_msgs::msg::Float64 vel_cmd_msg;
        vel_cmd_msg.data = output;
        velocity_setpoint_pub_->publish(vel_cmd_msg);

        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 200,
            "pitch=%.3f rad (%.1f deg) P=%.2f D=%.2f vel_cmd=%.2f rad/s",
            pitch, pitch * 180.0 / 3.14159,
            kp_ * error, kd_ * derivative, output);
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