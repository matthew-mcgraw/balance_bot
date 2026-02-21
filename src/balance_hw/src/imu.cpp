#include <rclcpp/rclcpp.hpp>
#include <fcntl.h> // open()
#include <unistd.h> // close(), read(), write()
#include <sys/ioctl.h> //ioctl()
#include <linux/i2c-dev.h> // I2C_SLAVE
#include <i2c/smbus.h>
#include <cstdint> // uint8_t, int16_t
#include <sensor_msgs/msg/imu.hpp>
#include <chrono>


class IMU_Node : public rclcpp::Node {
    public:
        IMU_Node() : rclcpp::Node("imu_node"){
            i2c_fd_ = ::open("/dev/i2c-1", O_RDWR);
            ioctl(i2c_fd_, I2C_SLAVE, i2c_addr);


        }

        ~IMU_Node() {

        }

    private:
        int i2c_fd_{-1};
        uint8_t i2c_addr_{0x68};

        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
        rclcpp::TimerBase::SharedPtr timer_;

}