#include <rclcpp/rclcpp.hpp>
#include <string>
#include <stdexcept>
#include <gpiod.h>

class MotorEncoderNode : public rclcpp::Node
{
public:
    MotorEncoderNode() : rclcpp::Node("motor_encoder_node")
    {
        this->declare_parameter<int>("motor_a_encoder_a", -1);
        this->declare_parameter<int>("motor_a_encoder_b", -1);
        this->declare_parameter<int>("motor_b_encoder_a", -1);
        this->declare_parameter<int>("motor_b_encoder_b", -1);
        this->declare_parameter<std::string>("chip_path", "");

        motor_a_encoder_a_ = this->get_parameter("motor_a_encoder_a").as_int();
        motor_a_encoder_b_ = this->get_parameter("motor_a_encoder_b").as_int();
        motor_b_encoder_a_ = this->get_parameter("motor_b_encoder_a").as_int();
        motor_b_encoder_b_ = this->get_parameter("motor_b_encoder_b").as_int();
        chip_path_ = this->get_parameter("chip_path").as_string();
        RCLCPP_INFO(get_logger(), "chip=%s motor_a_encoder_a=%d motor_a_encoder_b=%d motor_b_encoder_a=%d motor_b_encoder_b=%d",
                    chip_path_.c_str(), motor_a_encoder_a_, motor_a_encoder_b_, motor_b_encoder_a_, motor_b_encoder_b_);

        if (motor_a_encoder_a_ < 0 || motor_a_encoder_b_ < 0 || motor_b_encoder_a_ < 0 || motor_b_encoder_b_ < 0 || chip_path_.empty())
        {
            RCLCPP_FATAL(get_logger(), "Missing parameters (check --params-file and node name)");
            throw std::runtime_error("missing params");
        }

        chip_ = gpiod_chip_open(chip_path_.c_str());

        if (!chip_)
        {
            RCLCPP_FATAL(get_logger(), "Failed to open chip: %s", chip_path_.c_str());
            throw std::runtime_error("gpiod_chip_open failed");
        }

        motor_a_encoder_a_line_ = gpiod_chip_get_line(chip_, motor_a_encoder_a_);
        motor_a_encoder_b_line_ = gpiod_chip_get_line(chip_, motor_a_encoder_b_);
        motor_b_encoder_a_line_ = gpiod_chip_get_line(chip_, motor_b_encoder_a_);
        motor_b_encoder_b_line_ = gpiod_chip_get_line(chip_, motor_b_encoder_b_);

        if (!motor_a_encoder_a_line_ || !motor_a_encoder_b_line_ || !motor_b_encoder_a_line_ || !motor_b_encoder_b_line_)
        {
            RCLCPP_FATAL(get_logger(), "Failed to get GPIO lines for encoders");
            throw std::runtime_error("gpiod_chip_get_line failed");
        }

        init_gpio_input_line_SAFE(motor_a_encoder_a_line_, "motor_a_encoder_a");
        init_gpio_input_line_SAFE(motor_a_encoder_b_line_, "motor_a_encoder_b");
        init_gpio_input_line_SAFE(motor_b_encoder_a_line_, "motor_b_encoder_a");
        init_gpio_input_line_SAFE(motor_b_encoder_b_line_, "motor_b_encoder_b");
    }

    ~MotorEncoderNode()
    {
        if(chip_) gpiod_chip_close(chip_);
        if(motor_a_encoder_a_line_) gpiod_line_release(motor_a_encoder_a_line_);
        if(motor_a_encoder_b_line_) gpiod_line_release(motor_a_encoder_b_line_);
        if(motor_b_encoder_a_line_) gpiod_line_release(motor_b_encoder_a_line_);
        if(motor_b_encoder_b_line_) gpiod_line_release(motor_b_encoder_b_line_);
    }

private:
    gpiod_chip *chip_{nullptr};
    gpiod_line *motor_a_encoder_a_line_{nullptr};
    gpiod_line *motor_a_encoder_b_line_{nullptr};
    gpiod_line *motor_b_encoder_a_line_{nullptr};
    gpiod_line *motor_b_encoder_b_line_{nullptr};
    int motor_a_encoder_a_{-1};
    int motor_a_encoder_b_{-1};
    int motor_b_encoder_a_{-1};
    int motor_b_encoder_b_{-1};
    std::string chip_path_{""};

    void init_gpio_input_line_SAFE(gpiod_line *line, const char *label)
    {
        if (!line)
        {
            RCLCPP_FATAL(get_logger(), "%s line is null", label);
            throw std::runtime_error("null gpiod_line");
        }

        if (gpiod_line_request_both_edges_events(line, "balance_hw") < 0)
        {
            RCLCPP_FATAL(get_logger(), "Failed to request %s line as input", label);
            throw std::runtime_error(std::string("gpiod_line_request_both_edges_events ") + label + std::string(" failed"));
        }


    }

    // rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    // rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MotorEncoderNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}