#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <fcntl.h> // open()
#include <unistd.h> // close(), read(), write()
#include <sys/ioctl.h> //ioctl()
#include <linux/i2c-dev.h> // I2C_SLAVE
#include <cstdint> // uint8_t, int16_t
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <chrono> //create timer/wallclock
#include <stdexcept> 


class IMU_Node : public rclcpp::Node {
    public:
        IMU_Node() : rclcpp::Node("imu_node"){
            i2c_fd_ = ::open("/dev/i2c-1", O_RDWR);
            if(i2c_fd_ < 0){
                RCLCPP_FATAL(get_logger(), "Failed to open /dev/i2c-1");
                throw std::runtime_error("open(/dev/i2c-1) failed");
            }
            
            if(::ioctl(i2c_fd_, I2C_SLAVE, i2c_addr_) < 0){
                RCLCPP_FATAL(get_logger(), "Failed to set I2C_SLAVE addr=0x%02X", i2c_addr_);
                throw std::runtime_error("ioctl(I2C_SLAVE) failed");
            }

            uint8_t wake[2] = {0x6B, 0x00}; // PWR_MGMT_1 = 0x6B, write 0x00 to wake
            if(::write(i2c_fd_, wake, 2) != 2) {
                RCLCPP_FATAL(get_logger(), "Failed to wake MPU6050 (write PWR_MGMT_1)");
                throw std::runtime_error("MPU6050 wake write failed");
            }

            imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu/data_raw", 10);



            timer_ = this->create_wall_timer(
                std::chrono::milliseconds(5), // 200 Hz

                [this]() {
                    uint8_t buf[14];
                    if(!read_sample(buf)) return;

                    int16_t ax_raw = be16(buf[0], buf[1]);
                    int16_t ay_raw = be16(buf[2], buf[3]);
                    int16_t az_raw = be16(buf[4], buf[5]);

                    int16_t gx_raw = be16(buf[8], buf[9]);
                    int16_t gy_raw = be16(buf[10], buf[11]);
                    int16_t gz_raw = be16(buf[12], buf[13]);

                    // perform sample calibration
                    if(!calibrated_){
                        sum_ax_ += ax_raw;
                        sum_ay_ += ay_raw;
                        sum_az_ += az_raw;

                        sum_gx_ += gx_raw;
                        sum_gy_ += gy_raw;
                        sum_gz_ += gz_raw;

                        calib_count_++;

                        RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 500,
                            "Calibrating IMU... %d/%d", calib_count_, CALIBRATION_SAMPLE_COUNT_);

                        if (calib_count_ >= CALIBRATION_SAMPLE_COUNT_){
                            const double ax_avg = static_cast<double>(sum_ax_)/CALIBRATION_SAMPLE_COUNT_;
                            const double ay_avg = static_cast<double>(sum_ay_)/CALIBRATION_SAMPLE_COUNT_;
                            const double az_avg = static_cast<double>(sum_az_)/CALIBRATION_SAMPLE_COUNT_;
                            const double gx_avg = static_cast<double>(sum_gx_)/CALIBRATION_SAMPLE_COUNT_;
                            const double gy_avg = static_cast<double>(sum_gy_)/CALIBRATION_SAMPLE_COUNT_;
                            const double gz_avg = static_cast<double>(sum_gz_)/CALIBRATION_SAMPLE_COUNT_;
                            
                            bias_ax_ = ax_avg;
                            bias_ay_ = ay_avg;
                            bias_az_ = az_avg - 16384.0;
                            bias_gx_ = gx_avg;
                            bias_gy_ = gy_avg;
                            bias_gz_ = gz_avg;

                            calibrated_ = true;
                            RCLCPP_INFO(get_logger(),
                                "IMU calibrated. accel_bias(counts)=[%.1f %.1f %.1f], gyro_bias(counts)=[%.1f %.1f %.1f]",
                                bias_ax_, bias_ay_, bias_az_, bias_gx_, bias_gy_, bias_gz_);

                        }

                        return;

                    }

                                // bias correction
            const double ax_corr = static_cast<double>(ax_raw) - bias_ax_;
            const double ay_corr = static_cast<double>(ay_raw) - bias_ay_;
            const double az_corr = static_cast<double>(az_raw) - bias_az_;

            const double gx_corr = static_cast<double>(gx_raw) - bias_gx_;
            const double gy_corr = static_cast<double>(gy_raw) - bias_gy_;
            const double gz_corr = static_cast<double>(gz_raw) - bias_gz_;

            // convert corrected counts into SI units for sensor_msgs/Imu
            const double ax_ms2 = (ax_corr / accel_lsb_per_g) * g;
            const double ay_ms2 = (ay_corr / accel_lsb_per_g) * g;
            const double az_ms2 = (az_corr / accel_lsb_per_g) * g;

            const double gx_rads = (gx_corr / gyro_lsb_per_dps) * deg2rad;
            const double gy_rads = (gy_corr / gyro_lsb_per_dps) * deg2rad;
            const double gz_rads = (gz_corr / gyro_lsb_per_dps) * deg2rad;

            // publish ROS message
            sensor_msgs::msg::Imu msg;
            msg.header.stamp = now();
            msg.header.frame_id = "imu_link";

            msg.linear_acceleration.x = ax_ms2;
            msg.linear_acceleration.y = ay_ms2;
            msg.linear_acceleration.z = az_ms2;

            msg.angular_velocity.x = gx_rads;
            msg.angular_velocity.y = gy_rads;
            msg.angular_velocity.z = gz_rads;

            msg.orientation_covariance[0] = -1.0;

            if(!imu_pub_){
                RCLCPP_FATAL(get_logger(), "imu_pub_ is null (publisher not created)");
                return;
            }

            imu_pub_ ->publish(msg);

            RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 500,
                "accel(m/s^2)=[%.2f %.2f %.2f] gyro(rad/s)=[%.3f %.3f %.3f]",
                ax_ms2, ay_ms2, az_ms2, gx_rads, gy_rads, gz_rads);

                }
            );


        }

        ~IMU_Node() {
            if(i2c_fd_ >= 0) {
                ::close(i2c_fd_);
            }
        }

    private:
        int i2c_fd_{-1};
        const uint8_t i2c_addr_{0x68};
        bool calibrated_{false};
        const int CALIBRATION_SAMPLE_COUNT_{500};
        int calib_count_{0};
        int64_t sum_ax_{0};
        int64_t sum_ay_{0};
        int64_t sum_az_{0};
        int64_t sum_gx_{0};
        int64_t sum_gy_{0};
        int64_t sum_gz_{0};

        double bias_ax_{0};
        double bias_ay_{0};
        double bias_az_{0};
        double bias_gx_{0};
        double bias_gy_{0};
        double bias_gz_{0};

        const double g{9.807};
        const double accel_lsb_per_g{16384.0};
        const double gyro_lsb_per_dps{131.0};
        const double deg2rad{3.14159/180.0}; 

        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
        rclcpp::TimerBase::SharedPtr timer_;

        bool read_sample(uint8_t buf[14]) {
            uint8_t reg = 0x3B; // ACCEL_XOUT_H
            if (::write(i2c_fd_, &reg, 1) != 1) {
                RCLCPP_ERROR(get_logger(), "Failed to set MPU6050 register pointer (0x3B)");
                return false;
            }

            if(::read(i2c_fd_, buf, 14) != 14){
                RCLCPP_ERROR(get_logger(), "Failed to read 14 bytes from MPU6050");
                return false;
            }
            return true;
        }

        static int16_t be16(uint8_t hi, uint8_t lo){
            return static_cast<int16_t>((hi << 8) | lo);
        }

};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<IMU_Node>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}