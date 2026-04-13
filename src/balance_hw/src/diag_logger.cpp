#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <fstream>
#include <chrono>
#include <cmath>
#include <iomanip>

// Diagnostic logger: subscribes to all balance bot topics and prints
// a single CSV line at a fixed rate with the latest values from each.
// Also writes to a CSV file for offline analysis.

class DiagLogger : public rclcpp::Node
{
public:
    DiagLogger() : rclcpp::Node("diag_logger_node")
    {
        this->declare_parameter<double>("log_rate_hz", 20.0);
        this->declare_parameter<std::string>("csv_path", "/home/mmcgraw/balance_log.csv");

        double log_rate = this->get_parameter("log_rate_hz").as_double();
        std::string csv_path = this->get_parameter("csv_path").as_string();

        // Open CSV file
        csv_file_.open(csv_path, std::ios::out | std::ios::trunc);
        if (csv_file_.is_open()) {
            csv_file_ << "time_s,"
                       << "pitch_rad,pitch_deg,pitch_rate_rads,"
                       << "vel_setpoint_rads,wheel_vel_a_rads,wheel_vel_b_rads,wheel_vel_avg_rads,"
                       << "cmd_vel_norm,"
                       << "accel_x,accel_y,accel_z,"
                       << "gyro_x,gyro_y,gyro_z"
                       << std::endl;
            RCLCPP_INFO(get_logger(), "Logging to: %s at %.0f Hz", csv_path.c_str(), log_rate);
        } else {
            RCLCPP_ERROR(get_logger(), "Failed to open CSV file: %s", csv_path.c_str());
        }

        // Subscribe to all topics
        pitch_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/pitch", 10,
            [this](const std_msgs::msg::Float64::SharedPtr msg) {
                pitch_ = msg->data;
            });

        vel_setpoint_sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/velocity_setpoint", 10,
            [this](const std_msgs::msg::Float64::SharedPtr msg) {
                vel_setpoint_ = msg->data;
            });

        wheel_vel_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "/wheel_velocity", 10,
            [this](const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
                if (msg->data.size() >= 2) {
                    wheel_vel_a_ = msg->data[0];
                    wheel_vel_b_ = msg->data[1];
                }
            });

        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10,
            [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
                cmd_vel_ = msg->linear.x;
            });

        imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/imu/data_raw", 10,
            [this](const sensor_msgs::msg::Imu::SharedPtr msg) {
                accel_x_ = msg->linear_acceleration.x;
                accel_y_ = msg->linear_acceleration.y;
                accel_z_ = msg->linear_acceleration.z;
                gyro_x_ = msg->angular_velocity.x;
                gyro_y_ = msg->angular_velocity.y;
                gyro_z_ = msg->angular_velocity.z;
            });

        start_time_ = this->now();

        // Timer for periodic logging
        int period_ms = static_cast<int>(1000.0 / log_rate);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(period_ms),
            std::bind(&DiagLogger::on_timer, this));
    }

    ~DiagLogger() {
        if (csv_file_.is_open()) {
            csv_file_.close();
            RCLCPP_INFO(get_logger(), "CSV file closed.");
        }
    }

private:
    // Latest values from each topic
    double pitch_{0.0};
    double vel_setpoint_{0.0};
    double wheel_vel_a_{0.0};
    double wheel_vel_b_{0.0};
    double cmd_vel_{0.0};
    double accel_x_{0.0}, accel_y_{0.0}, accel_z_{0.0};
    double gyro_x_{0.0}, gyro_y_{0.0}, gyro_z_{0.0};

    rclcpp::Time start_time_;
    std::ofstream csv_file_;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr pitch_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr vel_setpoint_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr wheel_vel_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
    rclcpp::TimerBase::SharedPtr timer_;

    void on_timer()
    {
        const double t = (this->now() - start_time_).seconds();
        const double pitch_deg = pitch_ * 180.0 / 3.14159;
        const double wheel_avg = (wheel_vel_a_ + (-wheel_vel_b_)) / 2.0;

        // Console: compact single line
        RCLCPP_INFO(get_logger(),
            "t=%.1f | pitch=%+6.1f deg | sp=%+5.1f | whl=%+5.1f %+5.1f avg=%+5.1f | cmd=%+5.3f | gx=%+5.2f",
            t, pitch_deg, vel_setpoint_, wheel_vel_a_, wheel_vel_b_, wheel_avg, cmd_vel_, gyro_x_);

        // CSV: full precision
        if (csv_file_.is_open()) {
            csv_file_ << std::fixed << std::setprecision(4)
                       << t << ","
                       << pitch_ << "," << pitch_deg << "," << gyro_x_ << ","
                       << vel_setpoint_ << ","
                       << wheel_vel_a_ << "," << wheel_vel_b_ << "," << wheel_avg << ","
                       << cmd_vel_ << ","
                       << accel_x_ << "," << accel_y_ << "," << accel_z_ << ","
                       << gyro_x_ << "," << gyro_y_ << "," << gyro_z_
                       << "\n";
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DiagLogger>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}