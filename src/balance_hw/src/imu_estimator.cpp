#include <rclcpp/rclcpp.hpp> //ROS2 node API
#include <sensor_msgs/msg/imu.hpp> //imu message type to publish
#include <std_msgs/msg/float64.hpp>
#include <cmath>

class IMU_Estimator : public rclcpp::Node {
    public:
        IMU_Estimator() : rclcpp::Node("imu_estimator_node"){
            this->declare_parameter<double>("alpha", 0.98);

            alpha_ = this->get_parameter("alpha").as_double();

            RCLCPP_INFO(get_logger(), "imu_estimator_node initialized with alpha=%.3f", alpha_);

            imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
                "/imu/data_raw", 50,
                std::bind(&IMU_Estimator::on_imu, this, std::placeholders::_1)
            );
            pitch_pub_ = this->create_publisher<std_msgs::msg::Float64>("/pitch", 10);




        }

        ~IMU_Estimator() {

        }

    private:
        bool have_last_time_{false};
        rclcpp::Time last_time_;
        double pitch_rad_{0.0};
        double alpha_{0.98}; // closer to 1, use gyro estimation more, closer to 0, use accel estimation more

        //bias calibration
        bool bias_calibrated_{false};
        int bias_sample_count_{0};
        static constexpr int BIAS_SAMPLES = 200;
        double bias_sum_{0.0};
        double pitch_bias_{0.0};

        static constexpr int MA_WINDOW = 5;
        std::array<double, MA_WINDOW> ax_buf_{}, ay_buf_{}, az_buf_{}, gx_buf_{};
        int ma_idx_{0};
        int ma_count_{0};

        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
        rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pitch_pub_;

        void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg){
            const auto msg_time = rclcpp::Time(msg->header.stamp);

            

            if(!have_last_time_){
                last_time_ = msg_time;
                have_last_time_ = true;
                return;
            }
            if(msg_time.nanoseconds() ==  0) return;

            const double dt = (msg_time - last_time_).seconds(); //calculate time difference to integrate gyro measurement
            last_time_ = msg_time;

            // do not want to use negative time, or use a large delta,
            // balancing inverted pendulum smoothly requires very small theta, which also means very small time difference
            if(dt <= 0.0 || dt> 0.1) return; 

            const double ax = msg->linear_acceleration.x;
            const double ay = msg->linear_acceleration.y;
            const double az = msg->linear_acceleration.z;

            const double gx = msg->angular_velocity.x;

            ax_buf_[ma_idx_] = ax;
            ay_buf_[ma_idx_] = ay;
            az_buf_[ma_idx_] = az;
            gx_buf_[ma_idx_] = gx;
            ma_idx_ = (ma_idx_ + 1) % MA_WINDOW;
            if (ma_count_ < MA_WINDOW) ma_count_++;

            double ax_m = 0, ay_m = 0, az_m = 0, gx_m = 0;
            for(int i=0;i<ma_count_;i++){
                ax_m += ax_buf_[i];
                ay_m += ay_buf_[i];
                az_m += az_buf_[i];
                gx_m += gx_buf_[i];
            }
            ax_m /= ma_count_;
            ay_m /= ma_count_;
            az_m /= ma_count_;
            gx_m /= ma_count_;

            const double pitch_acc = std::atan2(-ay_m, std::sqrt(ax_m*ax_m + az_m*az_m));
            const double pitch_gyro = pitch_rad_ + gx_m*dt;

            pitch_rad_ = alpha_ * pitch_gyro + (1.0 - alpha_) * pitch_acc;

            if(!bias_calibrated_){
                bias_sum_ += pitch_rad_;
                bias_sample_count_++;
                if(bias_sample_count_ >= BIAS_SAMPLES){
                    pitch_bias_ = bias_sum_ / BIAS_SAMPLES;
                    bias_calibrated_ = true;
                    RCLCPP_INFO(get_logger(), "Pitch bias calibrated: %.4f rad (%.3f deg)",
                            pitch_bias_, pitch_bias_ * 180 / 3.14159);
                }
                return;
            }
            
            std_msgs::msg::Float64 out;
            out.data = pitch_rad_ - pitch_bias_;
            pitch_pub_->publish(out);

        }



};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<IMU_Estimator>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}