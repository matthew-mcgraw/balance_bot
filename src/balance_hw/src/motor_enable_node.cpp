#include <rclcpp/rclcpp.hpp>
#include <string>
#include <stdexcept>
#include <gpiod.h>
#include <std_srvs/srv/set_bool.hpp>
#include "balance_hw_interfaces/srv/set_direction.hpp"
#include "balance_hw_interfaces/srv/set_pwm.hpp"
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <geometry_msgs/msg/twist.hpp>
#include <chrono>


using SetDirection = balance_hw_interfaces::srv::SetDirection;
using SetPwm = balance_hw_interfaces::srv::SetPwm;

static bool write_sysfs_int(const std::string& path, int value){
    std::ofstream f(path);
    if(!f.is_open()) return false;
    f << value;
    return (bool)f;
}

class MotorEnableNode : public rclcpp::Node {
    public:
        MotorEnableNode() : rclcpp::Node("motor_enable_node"){
            this->declare_parameter<int>("pwm_a", -1);
            this->declare_parameter<int>("a_in1", -1);
            this->declare_parameter<int>("a_in2", -1);
            this->declare_parameter<int>("pwm_b", -1);
            this->declare_parameter<int>("b_in3", -1);
            this->declare_parameter<int>("b_in4", -1);
            this->declare_parameter<std::string>("chip_path", "");
            this->declare_parameter<std::string>("motor_mode", "");
            this->declare_parameter<int>("MIN_PWM", 300);

            pwm_a_ = this->get_parameter("pwm_a").as_int();
            a_in1_ = this->get_parameter("a_in1").as_int();
            a_in2_ = this->get_parameter("a_in2").as_int();
            pwm_b_ = this->get_parameter("pwm_b").as_int();
            b_in3_ = this->get_parameter("b_in3").as_int();
            b_in4_ = this->get_parameter("b_in4").as_int();
            chip_path_ = this->get_parameter("chip_path").as_string();
            motor_mode_ = this->get_parameter("motor_mode").as_string();
            MIN_PWM_ = this->get_parameter("MIN_PWM").as_int();
            RCLCPP_INFO(get_logger(), "chip=%s pwm_a=%d pwm_b=%d", chip_path_.c_str(), pwm_a_, pwm_b_);
            RCLCPP_INFO(get_logger(), "a_in1=%d a_in2=%d b_in3=%d b_in4=%d", a_in1_, a_in2_, b_in3_, b_in4_);
            RCLCPP_INFO(get_logger(), "MIN_PWM=%d", MIN_PWM_);

            if(pwm_a_ < 0 || pwm_b_ < 0 || a_in1_ < 0|| a_in2_ < 0  || b_in3_ < 0 || b_in4_ < 0 || chip_path_.empty() || motor_mode_.empty()){
                RCLCPP_FATAL(get_logger(), "Missing parameters (check --params-file and node name)");
                throw std::runtime_error("missing params");
            }

            if(motor_mode_ != "pwm" && motor_mode_ != "switch"){
                RCLCPP_FATAL(get_logger(), "Invalid motor mode, please set to pwm or switch");
                throw std::runtime_error("Invalid motor mode...");
            }

            chip_ = gpiod_chip_open(chip_path_.c_str());

            if(!chip_) {
                RCLCPP_FATAL(get_logger(),"Failed to open chip: %s", chip_path_.c_str());
                throw std::runtime_error("gpiod_chip_open failed");
            }

            if (motor_mode_ == "switch"){
                pwm_a_line_ = gpiod_chip_get_line(chip_, pwm_a_);
                pwm_b_line_ = gpiod_chip_get_line(chip_, pwm_b_);

                if (!pwm_a_line_ || !pwm_b_line_){
                    RCLCPP_FATAL(get_logger(),"Failed to get GPIO lines: %d, %d", pwm_a_, pwm_b_);
                    throw std::runtime_error("gpiod_chip_get_line failed");
                }

                init_gpio_line_SAFE(pwm_a_line_, "pwm_a");
                init_gpio_line_SAFE(pwm_b_line_, "pwm_b");
            }
            else{
                if (std::system("sudo -n /usr/local/bin/pwm_enable.sh 1000000") != 0){
                    RCLCPP_FATAL(get_logger(), "pwm_enable.sh failed (check sudoers or script)");
                    throw std::runtime_error("pwm init failed");
                }
            }

            a_in1_line_ = gpiod_chip_get_line(chip_, a_in1_);
            a_in2_line_ = gpiod_chip_get_line(chip_, a_in2_);
            b_in3_line_ = gpiod_chip_get_line(chip_, b_in3_);
            b_in4_line_ = gpiod_chip_get_line(chip_, b_in4_);

            if (!a_in1_line_ || !a_in2_line_){
                RCLCPP_FATAL(get_logger(),"Failed to get GPIO lines: %d, %d", a_in1_, a_in2_);
                throw std::runtime_error("gpiod_chip_get_line failed");
            }

            if (!b_in3_line_ || !b_in4_line_){
                RCLCPP_FATAL(get_logger(),"Failed to get GPIO lines: %d, %d", b_in3_, b_in4_);
                throw std::runtime_error("gpiod_chip_get_line failed");
            }

            init_gpio_line_SAFE(a_in1_line_, "a_in1");
            init_gpio_line_SAFE(a_in2_line_, "a_in2");
            init_gpio_line_SAFE(b_in3_line_, "b_in3");
            init_gpio_line_SAFE(b_in4_line_, "b_in4");

            using std::placeholders::_1;
            using std::placeholders::_2;

            motors_enable_srv_ = this->create_service<std_srvs::srv::SetBool>(
                "motors_enable",
                std::bind(&MotorEnableNode::on_motors_enable, this, _1, _2)
            );

            motors_direction_srv_ = this->create_service<SetDirection>(
                "motors_direction",
                std::bind(&MotorEnableNode::on_motors_direction, this, _1, _2)
            );

            motors_pwm_srv_ = this->create_service<SetPwm>(
                "motors_pwm",
                std::bind(&MotorEnableNode::on_motors_pwm, this, _1, _2)
            );

            cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
                "/cmd_vel",
                10,
                std::bind(&MotorEnableNode::on_cmd_vel, this, _1)
            );

            last_cmd_time_ = this->now();

            cmd_watchdog_timer_ = this->create_wall_timer(
                std::chrono::milliseconds(50),  // 20 Hz watchdog is plenty
                [this]() {
                    const auto age = (this->now() - last_cmd_time_).nanoseconds();
                    const int64_t timeout_ns = static_cast<int64_t>(cmd_timeout_ms_) * 1000000LL;

                    if (age > timeout_ns) {
                        v_cmd_ = 0.0;
                        w_cmd_ = 0.0;
                        pwm_cmd_ = 0.0;
                        set_direction(0.0);
                        set_pwm(0);
                    }
                }
            );
        }

        ~MotorEnableNode(){
            if (motor_mode_ == "switch"){
                if (pwm_a_line_) gpiod_line_release(pwm_a_line_);
                if (pwm_b_line_) gpiod_line_release(pwm_b_line_);
            }
            if (motor_mode_ == "pwm"){
                (void)std::system("sudo -n /usr/local/bin/pwm_disable.sh");
            }

            if (a_in1_line_) gpiod_line_release(a_in1_line_);
            if (a_in2_line_) gpiod_line_release(a_in2_line_);
            if (b_in3_line_) gpiod_line_release(b_in3_line_);
            if (b_in4_line_) gpiod_line_release(b_in4_line_);
            if (chip_) gpiod_chip_close(chip_);
        }

    private:
        gpiod_chip* chip_{nullptr};
        gpiod_line* pwm_a_line_{nullptr};
        gpiod_line* pwm_b_line_{nullptr};
        gpiod_line* a_in1_line_{nullptr};
        gpiod_line* a_in2_line_{nullptr};
        gpiod_line* b_in3_line_{nullptr};
        gpiod_line* b_in4_line_{nullptr};
        int pwm_a_{-1};
        int a_in1_{-1};
        int a_in2_{-1};
        int pwm_b_{-1};
        int b_in3_{-1};
        int b_in4_{-1};
        int MIN_PWM_{300};
        std::string motor_mode_{""};
        std::string chip_path_{""};
        rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr motors_enable_srv_;
        rclcpp::Service<SetDirection>::SharedPtr motors_direction_srv_;
        rclcpp::Service<SetPwm>::SharedPtr motors_pwm_srv_;
        int pwm_period_ns_{1000000};
        std::string pwmchip_path_{"/sys/class/pwm/pwmchip0"};
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
        double v_cmd_{0.0};
        double w_cmd_{0.0};
        double pwm_cmd_{0.0};
        rclcpp::Time last_cmd_time_;
        rclcpp::TimerBase::SharedPtr cmd_watchdog_timer_;
        int cmd_timeout_ms_{250};

        bool set_motor_a(bool enable){
            if (!pwm_a_line_) return false;
            if (gpiod_line_set_value(pwm_a_line_, enable ? 1 : 0) < 0) return false;
            return true;
        }

        bool set_motor_b(bool enable){
            if (!pwm_b_line_) return false;
            if (gpiod_line_set_value(pwm_b_line_, enable ? 1 : 0) < 0) return false;
            return true;
        }

        bool set_input_line(gpiod_line* line, bool enable, const char* label = "unknown"){
            if (!line) return false;
            if (gpiod_line_set_value(line, enable ? 1 : 0) < 0) return false;
            return true;
        }

        void init_gpio_line_SAFE(gpiod_line* line, const char* label){
            if (!line){
                RCLCPP_FATAL(get_logger(), "%s line is null", label);
                throw std::runtime_error("null gpiod_line");
            }

            if (gpiod_line_request_output(line, "balance_hw", 0) < 0){
                RCLCPP_FATAL(get_logger(), "Failed to request %s line as output", label);
                throw std::runtime_error(std::string("gpiod_line_request_output ") + label + std::string(" failed"));
            }

            if (gpiod_line_set_value(line, 0) < 0){
                RCLCPP_FATAL(get_logger(), "Failed to set %s line value", label);
                throw std::runtime_error(std::string("gpiod_line_set_value ") + label + std::string(" failed"));
            }
        }

        void on_motors_enable(
            const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
            std::shared_ptr<std_srvs::srv::SetBool::Response> res){

                const bool enable = req->data;
                const bool ok_a = set_input_line(pwm_a_line_, enable, "pwm_a");
                const bool ok_b = set_input_line(pwm_b_line_, enable, "pwm_b");
                res->success = ok_a && ok_b;
                res->message = res->success ? "Motors updated" : "Failed to update one or more motors";
        }

        void on_motors_direction(
            const std::shared_ptr<SetDirection::Request> req,
            std::shared_ptr<SetDirection::Response> res){
                last_cmd_time_ = this->now();
                const auto direction = req->direction;
                bool ok_ain1 = false, ok_ain2 = false, ok_bin3 = false, ok_bin4 = false;

                if(direction == 1){
                    ok_ain1 = set_input_line(a_in1_line_, true, "a_in1");
                    ok_ain2 = set_input_line(a_in2_line_, false, "a_in2");
                    ok_bin3 = set_input_line(b_in3_line_, true, "b_in3");
                    ok_bin4 = set_input_line(b_in4_line_, false, "b_in4");
                }
                else if(direction == -1){
                    ok_ain1 = set_input_line(a_in1_line_, false, "a_in1");
                    ok_ain2 = set_input_line(a_in2_line_, true, "a_in2");
                    ok_bin3 = set_input_line(b_in3_line_, false, "b_in3");
                    ok_bin4 = set_input_line(b_in4_line_, true, "b_in4");
                }
                else if(direction == 0){
                    ok_ain1 = set_input_line(a_in1_line_, false, "a_in1");
                    ok_ain2 = set_input_line(a_in2_line_, false, "a_in2");
                    ok_bin3 = set_input_line(b_in3_line_, false, "b_in3");
                    ok_bin4 = set_input_line(b_in4_line_, false, "b_in4");
                }
                else{
                    ok_ain1 = set_input_line(a_in1_line_, false, "a_in1");
                    ok_ain2 = set_input_line(a_in2_line_, false, "a_in2");
                    ok_bin3 = set_input_line(b_in3_line_, false, "b_in3");
                    ok_bin4 = set_input_line(b_in4_line_, false, "b_in4");
                    res->success = false;
                    res->message = "Invalid direction: use -1, 0, or 1";
                    return;
                }

                res->success = ok_ain1 && ok_ain2 && ok_bin3 && ok_bin4;
                res->message = res->success ? "Motor direction updated" : "Failed to update direction for one or more motors";
        }

        void on_motors_pwm(
            const std::shared_ptr<SetPwm::Request> req,
            std::shared_ptr<SetPwm::Response> res
        ){
            last_cmd_time_ = this->now();
            if(motor_mode_ != "pwm"){
                res->success = false;
                res->message = "Not in pwm mode";
                return;
            }

            int pwm_a = std::clamp<int>(req->pwm_a, 0, 1000);
            int pwm_b = std::clamp<int>(req->pwm_b, 0, 1000);

            int duty_a = (pwm_period_ns_ * pwm_a) / 1000;
            int duty_b = (pwm_period_ns_ * pwm_b) / 1000;

            bool ok0 = write_sysfs_int(pwmchip_path_ + "/pwm0/duty_cycle", duty_a);
            bool ok1 = write_sysfs_int(pwmchip_path_ + "/pwm1/duty_cycle", duty_b);

            res->success = ok0 && ok1;
            res->message = res->success ? "Motor PWMs updated" : "Failed to write PWM duty";
        }

        void on_cmd_vel(const geometry_msgs::msg::Twist::SharedPtr msg){
            last_cmd_time_ = this->now();

            if(motor_mode_ != "pwm"){
                RCLCPP_WARN(get_logger(), "Received cmd_vel but not in pwm mode, ignoring");
                return;
            }

            pwm_cmd_ = std::clamp(msg->linear.x, -1.0, 1.0);
            set_direction(pwm_cmd_);

            int pwm = static_cast<int>(std::abs(pwm_cmd_) * 1000);

            // Dead zone: below a tiny threshold, motor is off (coast).
            // Above threshold, apply MIN_PWM floor so motor actually turns.
            if (pwm > 0 && pwm < 10) {
                pwm = 0;  // treat < 1% as zero
            } else if (pwm >= 10 && pwm < MIN_PWM_) {
                pwm = MIN_PWM_;
            }

            set_pwm_raw(pwm);
        }

        void set_direction(double v_cmd){
                if(v_cmd > 0.0005){
                    set_input_line(a_in1_line_, true, "a_in1");
                    set_input_line(a_in2_line_, false, "a_in2");
                    set_input_line(b_in3_line_, true, "b_in3");
                    set_input_line(b_in4_line_, false, "b_in4");
                }
                else if(v_cmd < -0.0005){
                    set_input_line(a_in1_line_, false, "a_in1");
                    set_input_line(a_in2_line_, true, "a_in2");
                    set_input_line(b_in3_line_, false, "b_in3");
                    set_input_line(b_in4_line_, true, "b_in4");
                }
                else{
                    set_input_line(a_in1_line_, false, "a_in1");
                    set_input_line(a_in2_line_, false, "a_in2");
                    set_input_line(b_in3_line_, false, "b_in3");
                    set_input_line(b_in4_line_, false, "b_in4");
                }
        }

        void set_pwm(int pwm_value){
            int pwm = std::clamp<int>(pwm_value, 0, 1000);

            // MIN_PWM floor only for service-based calls (manual control)
            if (pwm > 0 && pwm < MIN_PWM_) pwm = MIN_PWM_;

            int duty_a = (pwm_period_ns_ * pwm) / 1000;
            int duty_b = (pwm_period_ns_ * pwm) / 1000;

            write_sysfs_int(pwmchip_path_ + "/pwm0/duty_cycle", duty_a);
            write_sysfs_int(pwmchip_path_ + "/pwm1/duty_cycle", duty_b);
        }

        // Direct PWM write — dead-zone compensation already handled by caller
        void set_pwm_raw(int pwm_value){
            int pwm = std::clamp<int>(pwm_value, 0, 1000);

            int duty_a = (pwm_period_ns_ * pwm) / 1000;
            int duty_b = (pwm_period_ns_ * pwm) / 1000;

            write_sysfs_int(pwmchip_path_ + "/pwm0/duty_cycle", duty_a);
            write_sysfs_int(pwmchip_path_ + "/pwm1/duty_cycle", duty_b);
        }
};

int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MotorEnableNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}