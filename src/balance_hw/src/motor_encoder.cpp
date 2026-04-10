#include <rclcpp/rclcpp.hpp>
#include <string>
#include <stdexcept>
#include <gpiod.h>
#include <mutex>
#include <thread>
#include <atomic>
#include <std_msgs/msg/float64_multi_array.hpp>

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

        // Start the encoder counting threads. Each thread runs independently,
        // blocking on GPIO events for its respective motor's A channel.
        // std::bind creates a callable that points to the member function on this instance.
        thread_a_ = std::thread(std::bind(&MotorEncoderNode::encoder_thread_a, this));
        thread_b_ = std::thread(std::bind(&MotorEncoderNode::encoder_thread_b, this));

        last_time_ = this->now();

        encoder_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("/wheel_velocity", 10);

        timer_ = this->create_wall_timer(
            std::chrono::microseconds(2500), // 400 Hz
            [this]()
            {
                const auto age = (this->now() - last_time_).nanoseconds();

                if (age <= 0)
                {
                    last_time_ = this->now();
                    return;
                }
                if (!encoder_pub_)
                {
                    RCLCPP_FATAL(get_logger(), "encoder_pub_ is null (publisher not created)");
                    return;
                }

                std::lock_guard<std::mutex> lock(count_mutex_);
                const int64_t count_a = motor_a_count_;
                const int64_t count_b = motor_b_count_;
                motor_a_count_ = 0;
                motor_b_count_ = 0;
                auto msg = std_msgs::msg::Float64MultiArray();

                double motor_a_velocity = (static_cast<double>(count_a) / 1632) * (2 * 3.14159) / (static_cast<double>(age) / 1e9); // Convert counts to radians per second
                double motor_b_velocity = (static_cast<double>(count_b) / 1632) * (2 * 3.14159) / (static_cast<double>(age) / 1e9); // Convert counts to radians per second
                RCLCPP_INFO_THROTTLE(get_logger(), *this->get_clock(), 500,
                                     "Motor A velocity: %.2f rad/s, Motor B velocity: %.2f rad/s", motor_a_velocity, motor_b_velocity);

                msg.data.push_back(static_cast<double>(motor_a_velocity));
                msg.data.push_back(static_cast<double>(motor_b_velocity));

                encoder_pub_->publish(msg);

                last_time_ = this->now();
            });
    }

    ~MotorEncoderNode()
    {

        running_ = false;
        if (thread_a_.joinable())
            thread_a_.join();
        if (thread_b_.joinable())
            thread_b_.join();

        if (chip_)
            gpiod_chip_close(chip_);
        if (motor_a_encoder_a_line_)
            gpiod_line_release(motor_a_encoder_a_line_);
        if (motor_a_encoder_b_line_)
            gpiod_line_release(motor_a_encoder_b_line_);
        if (motor_b_encoder_a_line_)
            gpiod_line_release(motor_b_encoder_a_line_);
        if (motor_b_encoder_b_line_)
            gpiod_line_release(motor_b_encoder_b_line_);
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
    int64_t motor_a_count_{0};
    int64_t motor_b_count_{0};
    std::mutex count_mutex_;
    std::thread thread_a_;
    std::thread thread_b_;
    std::atomic<bool> running_{true};
    rclcpp::Time last_time_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr encoder_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

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

    void encoder_thread_a()
    {
        // gpiod_line_event struct holds the details of a single GPIO event:
        // - event_type: GPIOD_LINE_EVENT_RISING_EDGE or GPIOD_LINE_EVENT_FALLING_EDGE
        // - ts: timestamp of when the event occurred
        struct gpiod_line_event event;

        // timespec is a standard C struct for specifying time intervals:
        // - tv_sec: whole seconds
        // - tv_nsec: nanoseconds (0 to 999,999,999)
        // This is passed to gpiod_line_event_wait as the maximum time to block
        // before giving up and looping again. 1 second means if no encoder pulse
        // arrives for 1 second, the wait returns and we check running_ again.
        // This is important for clean shutdown — without a timeout, the thread
        // would block forever on a stopped motor and never see running_ = false.
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = 10'000'000; // 10 ms

        // Loop until the node is shutting down. running_ is a std::atomic<bool>
        // set to false in the destructor, which causes this thread to exit cleanly.
        while (running_)
        {
            // Block this thread until a GPIO edge event occurs on the A channel,
            // or until the timeout expires (whichever comes first).
            // Returns:
            //   1  if an event is ready to read
            //   0  if the timeout expired with no event (we just loop again)
            //  -1  on error
            // We're not checking the return value here — a more robust implementation
            // would check for -1 and log an error.
            gpiod_line_event_wait(motor_a_encoder_a_line_, &timeout);

            // Read and consume the pending event from the A channel line.
            // This clears the event from the kernel buffer so the next call to
            // gpiod_line_event_wait will block fresh. The event details are
            // written into our local `event` struct.
            gpiod_line_event_read(motor_a_encoder_a_line_, &event);

            // ← add this block
            const int64_t event_time_ns = event.ts.tv_sec * 1000000000LL + event.ts.tv_nsec;
            if (event_time_ns - last_event_time_ns < 50000)
            {
                continue; // reject as noise, too close to previous event
            }
            last_event_time_ns = event_time_ns;

            // Quadrature decoding on the RISING edge of channel A:
            // When A rises, the state of B tells us the direction of rotation.
            //
            // Forward rotation waveform:
            //   A: ___/‾‾‾\___/‾‾‾\___
            //   B: _/‾‾‾\___/‾‾‾\_____
            //   B leads A by 90° — so when A rises, B is already LOW
            //
            // Backward rotation waveform:
            //   A: _/‾‾‾\___/‾‾‾\____
            //   B: ‾‾‾\___/‾‾‾\___/‾‾
            //   A leads B by 90° — so when A rises, B is HIGH
            //
            // Therefore: A rises + B LOW = forward, A rises + B HIGH = backward
            if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
            {
                // Read the instantaneous state of channel B.
                // gpiod_line_get_value returns 0 (LOW) or 1 (HIGH).
                // Note: motor_a_encoder_b_line_ must also be initialized as an
                // input — it was requested with gpiod_line_request_both_edges_events
                // in the constructor, which also allows reading its current value.
                const int b_val = gpiod_line_get_value(motor_a_encoder_b_line_);

                // Lock the mutex before modifying motor_a_count_.
                // The velocity timer on the main thread also reads motor_a_count_,
                // so we need mutual exclusion to prevent a data race.
                // std::lock_guard automatically releases the lock when it goes
                // out of scope at the end of this if block.
                std::lock_guard<std::mutex> lock(count_mutex_);

                if (b_val > 0)
                {
                    // B is HIGH when A rises → backward rotation
                    motor_a_count_--;
                }
                else
                {
                    // B is LOW when A rises → forward rotation
                    motor_a_count_++;
                }
            }

            // Quadrature decoding on the FALLING edge of channel A:
            // The direction logic is the INVERSE of the rising edge case.
            //
            // Forward rotation: when A falls, B is HIGH
            // Backward rotation: when A falls, B is LOW
            //
            // This gives us 2x more counts per revolution compared to only
            // counting rising edges, improving velocity resolution at low speeds.
            // Combined with also counting B edges (not done here) you can get
            // 4x counts — called "full quadrature" or "4x decoding".
            else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE)
            {
                const int b_val = gpiod_line_get_value(motor_a_encoder_b_line_);
                std::lock_guard<std::mutex> lock(count_mutex_);

                if (b_val > 0)
                {
                    // B is HIGH when A falls → forward rotation
                    motor_a_count_++;
                }
                else
                {
                    // B is LOW when A falls → backward rotation
                    motor_a_count_--;
                }
            }
            // Note: if gpiod_line_event_wait timed out (no event), event_type will
            // not match either case above and we simply loop back to wait again.
        }
    }

    void encoder_thread_b()
    {
        // This thread is essentially the same as encoder_thread_a but for motor B.
        // It waits for events on motor_b_encoder_a_line_ and reads motor_b_encoder_b_line_
        // to do quadrature decoding and update motor_b_count_.
        // For brevity, the implementation is omitted here, but it would mirror
        // encoder_thread_a with appropriate variable substitutions.
        // gpiod_line_event struct holds the details of a single GPIO event:
        // - event_type: GPIOD_LINE_EVENT_RISING_EDGE or GPIOD_LINE_EVENT_FALLING_EDGE
        // - ts: timestamp of when the event occurred
        struct gpiod_line_event event;

        // timespec is a standard C struct for specifying time intervals:
        // - tv_sec: whole seconds
        // - tv_nsec: nanoseconds (0 to 999,999,999)
        // This is passed to gpiod_line_event_wait as the maximum time to block
        // before giving up and looping again. 1 second means if no encoder pulse
        // arrives for 1 second, the wait returns and we check running_ again.
        // This is important for clean shutdown — without a timeout, the thread
        // would block forever on a stopped motor and never see running_ = false.
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = 10'000'000; // 10 ms

        // Loop until the node is shutting down. running_ is a std::atomic<bool>
        // set to false in the destructor, which causes this thread to exit cleanly.
        while (running_)
        {
            // Block this thread until a GPIO edge event occurs on the A channel,
            // or until the timeout expires (whichever comes first).
            // Returns:
            //   1  if an event is ready to read
            //   0  if the timeout expired with no event (we just loop again)
            //  -1  on error
            // We're not checking the return value here — a more robust implementation
            // would check for -1 and log an error.
            gpiod_line_event_wait(motor_b_encoder_a_line_, &timeout);

            // Read and consume the pending event from the A channel line.
            // This clears the event from the kernel buffer so the next call to
            // gpiod_line_event_wait will block fresh. The event details are
            // written into our local `event` struct.
            gpiod_line_event_read(motor_b_encoder_a_line_, &event);

            // ← add this block
            const int64_t event_time_ns = event.ts.tv_sec * 1000000000LL + event.ts.tv_nsec;
            if (event_time_ns - last_event_time_ns < 50000)
            {
                continue; // reject as noise, too close to previous event
            }
            last_event_time_ns = event_time_ns;

            // Quadrature decoding on the RISING edge of channel A:
            // When A rises, the state of B tells us the direction of rotation.
            //
            // Forward rotation waveform:
            //   A: ___/‾‾‾\___/‾‾‾\___
            //   B: _/‾‾‾\___/‾‾‾\_____
            //   B leads A by 90° — so when A rises, B is already LOW
            //
            // Backward rotation waveform:
            //   A: _/‾‾‾\___/‾‾‾\____
            //   B: ‾‾‾\___/‾‾‾\___/‾‾
            //   A leads B by 90° — so when A rises, B is HIGH
            //
            // Therefore: A rises + B LOW = forward, A rises + B HIGH = backward
            if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
            {
                // Read the instantaneous state of channel B.
                // gpiod_line_get_value returns 0 (LOW) or 1 (HIGH).
                // Note: motor_b_encoder_b_line_ must also be initialized as an
                // input — it was requested with gpiod_line_request_both_edges_events
                // in the constructor, which also allows reading its current value.
                const int b_val = gpiod_line_get_value(motor_b_encoder_b_line_);

                // Lock the mutex before modifying motor_b_count_.
                // The velocity timer on the main thread also reads motor_b_count_,
                // so we need mutual exclusion to prevent a data race.
                // std::lock_guard automatically releases the lock when it goes
                // out of scope at the end of this if block.
                std::lock_guard<std::mutex> lock(count_mutex_);

                if (b_val > 0)
                {
                    // B is HIGH when A rises → backward rotation
                    motor_b_count_--;
                }
                else
                {
                    // B is LOW when A rises → forward rotation
                    motor_b_count_++;
                }
            }

            // Quadrature decoding on the FALLING edge of channel A:
            // The direction logic is the INVERSE of the rising edge case.
            //
            // Forward rotation: when A falls, B is HIGH
            // Backward rotation: when A falls, B is LOW
            //
            // This gives us 2x more counts per revolution compared to only
            // counting rising edges, improving velocity resolution at low speeds.
            // Combined with also counting B edges (not done here) you can get
            // 4x counts — called "full quadrature" or "4x decoding".
            else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE)
            {
                const int b_val = gpiod_line_get_value(motor_b_encoder_b_line_);
                std::lock_guard<std::mutex> lock(count_mutex_);

                if (b_val > 0)
                {
                    // B is HIGH when A falls → forward rotation
                    motor_b_count_++;
                }
                else
                {
                    // B is LOW when A falls → backward rotation
                    motor_b_count_--;
                }
            }
            // Note: if gpiod_line_event_wait timed out (no event), event_type will
            // not match either case above and we simply loop back to wait again.
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MotorEncoderNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}