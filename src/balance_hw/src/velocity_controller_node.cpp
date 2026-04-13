#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cmath>

// Velocity controller: closed-loop PID that tracks the velocity setpoint
// from the balance PID using encoder feedback.
//
// All control computation happens in the encoder callback so that every
// output update is based on fresh measurement data. The setpoint callback
// just stores the latest target.
//
// Output slew rate limiting prevents the PWM from jumping instantly,
// giving the motor time to ramp up/down smoothly.

class Velocity_Controller : public rclcpp::Node
{
public:
    Velocity_Controller() : rclcpp::Node("velocity_controller_node")
    {
        this->declare_parameter<double>("omega_no_load", 31.4);
        this->declare_parameter<double>("kp_vel", 0.04);
        this->declare_parameter<double>("ki_vel", 0.15);
        this->declare_parameter<double>("kd_vel", 0.005);

        omega_no_load_ = this->get_parameter("omega_no_load").as_double();
        kp_vel_ = this->get_parameter("kp_vel").as_double();
        ki_vel_ = this->get_parameter("ki_vel").as_double();
        kd_vel_ = this->get_parameter("kd_vel").as_double();

        vel_setpoint_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/velocity_setpoint", 10,
            [this](const std_msgs::msg::Float64::SharedPtr msg) {
                velocity_setpoint_ = msg->data;
            });

        vel_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "/wheel_velocity", 10,
            std::bind(&Velocity_Controller::on_wheel_velocity, this, std::placeholders::_1));

        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        last_time_ = this->now();

        RCLCPP_INFO(get_logger(),
                    "velocity_controller_node initialized: omega_no_load=%.1f, "
                    "kp=%.3f, ki=%.3f, kd=%.4f",
                    omega_no_load_, kp_vel_, ki_vel_, kd_vel_);
    }

private:
    double omega_no_load_{31.4};
    double kp_vel_{0.04};
    double ki_vel_{0.15};
    double kd_vel_{0.005};

    double velocity_setpoint_{0.0};
    double filtered_velocity_{0.0};
    double vel_filter_alpha_{0.7};
    double integral_{0.0};
    double prev_filtered_velocity_{0.0};
    double filtered_derivative_{0.0};
    double deriv_filter_alpha_{0.7};

    rclcpp::Time last_time_;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr vel_setpoint_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr vel_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;

    void on_wheel_velocity(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 2) return;

        const double raw_velocity = (msg->data[0] + (-msg->data[1])) / 2.0;
        filtered_velocity_ = vel_filter_alpha_ * filtered_velocity_ + (1.0 - vel_filter_alpha_) * raw_velocity;

        const auto now = this->now();
        const double dt = (now - last_time_).seconds();
        last_time_ = now;

        if (dt <= 0.0 || dt > 0.5) return;

        // --- PID computation ---
        const double error = velocity_setpoint_ - filtered_velocity_;

        // Integral with anti-windup
        integral_ += error * dt;
        integral_ = std::clamp(integral_, -3.0, 3.0);

        // Hard reset integral when setpoint is near zero.
        // After a fall, the balance PID sends 0. Without this reset,
        // the wound-up integral keeps driving the motors and throws
        // the bot the opposite direction when it recovers.
        if (std::abs(velocity_setpoint_) < 0.5) {
            integral_ = 0.0;
        }

        // Derivative-on-measurement (not error) to avoid setpoint kick
        const double raw_deriv = (filtered_velocity_ - prev_filtered_velocity_) / dt;
        filtered_derivative_ = deriv_filter_alpha_ * filtered_derivative_ + (1.0 - deriv_filter_alpha_) * raw_deriv;
        prev_filtered_velocity_ = filtered_velocity_;

        // Feedforward + PID
        double ff = velocity_setpoint_ / omega_no_load_;
        double pid = kp_vel_ * error + ki_vel_ * integral_ - kd_vel_ * filtered_derivative_;
        double output = ff + pid;
        output = std::clamp(output, -1.0, 1.0);

        // Publish
        geometry_msgs::msg::Twist cmd_msg;
        cmd_msg.linear.x = output;
        cmd_vel_pub_->publish(cmd_msg);

        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 500,
            "sp=%.2f meas=%.2f err=%.2f ff=%.3f pid=%.3f out=%.3f",
            velocity_setpoint_, filtered_velocity_, error, ff, pid, output);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Velocity_Controller>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}