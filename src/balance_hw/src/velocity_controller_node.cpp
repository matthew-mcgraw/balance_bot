#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cmath>

class Velocity_Controller : public rclcpp::Node
{
public:
    Velocity_Controller() : rclcpp::Node("velocity_controller_node")
    {
        this->declare_parameter<double>("wheel_radius", 0.045);
        this->declare_parameter<double>("motor_stall_torque", 1.079);
        this->declare_parameter<double>("omega_no_load", 31.4);
        this->declare_parameter<double>("kp_vel", 1.0);
        this->declare_parameter<double>("ki_vel", 0.0);
        this->declare_parameter<double>("kd_vel", 0.0);

        wheel_radius_ = this->get_parameter("wheel_radius").as_double();
        motor_stall_torque_ = this->get_parameter("motor_stall_torque").as_double();
        omega_no_load_ = this->get_parameter("omega_no_load").as_double();
        kp_vel_ = this->get_parameter("kp_vel").as_double();
        ki_vel_ = this->get_parameter("ki_vel").as_double();
        kd_vel_ = this->get_parameter("kd_vel").as_double();

        force_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/force_setpoint", 10,
            std::bind(&Velocity_Controller::on_force_setpoint, this, std::placeholders::_1));

        vel_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "/wheel_velocity", 10,
            std::bind(&Velocity_Controller::on_wheel_velocity, this, std::placeholders::_1));

        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        last_time_ = this->now();

        RCLCPP_INFO(get_logger(), "velocity_controller_node initialized with wheel_radius=%.3f m, motor_stall_torque=%.3f N-m, omega_no_load=%.1f rad/s, kp_vel=%.2f, ki_vel=%.2f, kd_vel=%.2f",
                    wheel_radius_, motor_stall_torque_, omega_no_load_, kp_vel_, ki_vel_, kd_vel_);
    }

    ~Velocity_Controller()
    {
    }

private:
    // Parameters for velocity control
    double wheel_radius_{0.045};       // in meters, adjust based on your actual wheel radius
    double motor_stall_torque_{1.079}; // in N-m, adjust based on your motor's stall torque
    double omega_no_load_{31.4};       // in rad/s, adjust based on your motor's no-load speed at 12V
    double kp_vel_{1.0};               // Proportional gain for velocity control, adjust based on your robot's response (start with a high value and decrease if the system oscillates)
    double ki_vel_{0.0};               // Integral gain for velocity control, can be set to 0 for a start, increase if you see steady-state error (robot not reaching desired velocity), but be careful as too high value can cause instability
    double kd_vel_{0.0};               // Derivative gain for velocity control, can be

    // State
    double force_setpoint_{0.0};    // latest force setpoint from the PID controller
    double omega_setpoint_{0.0};    // desired wheel angular velocity setpoint calculated from the force setpoint
    double measured_velocity_{0.0}; // latest measured velocity of wheel A from the encoder node
    double integral_{0.0};          // integral term for velocity PID control
    double prev_error_{0.0};        // previous error for derivative term in velocity PID control
    bool have_prev_{false};         // flag to check if we have a previous error value for derivative calculation
    rclcpp::Time last_time_;        // last time we received a velocity measurement, used for calculating dt in PID control

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr force_sub_;         // subscribe to the force command to calculate the PID output
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr vel_sub_; // subscribe to the estimated pitch angle to calculate the PID output
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;       // publish cmd_vel with linear.x set to the PWM output for the motor controller node to consume

    void on_force_setpoint(const std_msgs::msg::Float64::SharedPtr msg)
    {
        force_setpoint_ = msg->data;

        // Convert  force (newetons) to desired wheel angular velocity (rad/s) using the formula: torque = force * wheel_radius, and then using the motor's torque curve to find the corresponding angular velocity
        double torque_setpoint = force_setpoint_ * wheel_radius_; // Torque = Force x Radius
                                                                  // Using a simple linear approximation of the motor's torque curve: omega = omega_no_load * (1 - torque / motor_stall_torque)
        omega_setpoint_ = (force_setpoint_ * wheel_radius_) / motor_stall_torque_ * omega_no_load_;
        RCLCPP_DEBUG(get_logger(), "Received force setpoint: %.3f N, calculated torque setpoint: %.3f N-m, resulting in omega setpoint: %.2f rad/s", force_setpoint_, torque_setpoint, omega_setpoint_);
    }

    void on_wheel_velocity(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 2)
        {
            RCLCPP_WARN(get_logger(), "wheel_velocity message has fewer than 2 elements");
            return;
        }

        // average both wheel velocities for a single speed estimate
        // sign convention: positive = forward
        measured_velocity_ = (msg->data[0] + (-msg->data[1])) / 2.0;

        // compute dt
        const auto now = this->now();
        const double dt = (now - last_time_).seconds();
        last_time_ = now;

        // guard against bad dt on first sample or after gap
        if (!have_prev_ || dt <= 0.0 || dt > 0.1)
        {
            integral_ = 0.0;
            prev_error_ = 0.0;
            have_prev_ = true;
            RCLCPP_DEBUG(get_logger(), "Resetting PID state due to invalid dt: %.3f seconds", dt);
            return;
        }

        if (std::abs(omega_setpoint_) < 0.05)
        {
            integral_ = 0.0;
            geometry_msgs::msg::Twist cmd_msg;
            cmd_msg.linear.x = 0.0;
            cmd_vel_pub_->publish(cmd_msg);
            return;
        }
        // Velocity PID control
        // error: how far measured wheel speed is from setpoint
        const double error = omega_setpoint_ - measured_velocity_;

        // ingetral with anti-windup clamp
        integral_ += error * dt;
        integral_ = std::clamp(integral_, -10.0, 10.0); // adjust clamp limits as needed

        // derivative on measurement to avoid derivative kick on setpoint changes
        const double derivative = (error - prev_error_) / dt;
        prev_error_ = error;

        // pid output - this is a normalized pwm command in [-1, 1]
        double output = (kp_vel_ * error) + (ki_vel_ * integral_) + (kd_vel_ * derivative);

        // clamp output to valid range for motor command
        output = std::clamp(output, -1.0, 1.0);

        // publish to motor_enable_node via /cmd_vel
        // linear.x carries the normalized torque command [-1, 1], not an actual velocity in m/s, since the motor controller node is doing the final conversion to PWM based on the motor model
        geometry_msgs::msg::Twist cmd_vel_msg;
        cmd_vel_msg.linear.x = output;
        cmd_vel_pub_->publish(cmd_vel_msg);

        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 500,
                             "Force setpoint: %.3f N, Omega setpoint: %.2f rad/s, Measured velocity: %.2f rad/s, PID output (normalized): %.2f",
                             force_setpoint_, omega_setpoint_, measured_velocity_, output);
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