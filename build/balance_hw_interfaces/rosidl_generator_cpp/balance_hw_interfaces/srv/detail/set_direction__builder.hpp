// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from balance_hw_interfaces:srv/SetDirection.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "balance_hw_interfaces/srv/set_direction.hpp"


#ifndef BALANCE_HW_INTERFACES__SRV__DETAIL__SET_DIRECTION__BUILDER_HPP_
#define BALANCE_HW_INTERFACES__SRV__DETAIL__SET_DIRECTION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "balance_hw_interfaces/srv/detail/set_direction__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace balance_hw_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetDirection_Request_direction
{
public:
  Init_SetDirection_Request_direction()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::balance_hw_interfaces::srv::SetDirection_Request direction(::balance_hw_interfaces::srv::SetDirection_Request::_direction_type arg)
  {
    msg_.direction = std::move(arg);
    return std::move(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::balance_hw_interfaces::srv::SetDirection_Request>()
{
  return balance_hw_interfaces::srv::builder::Init_SetDirection_Request_direction();
}

}  // namespace balance_hw_interfaces


namespace balance_hw_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetDirection_Response_message
{
public:
  explicit Init_SetDirection_Response_message(::balance_hw_interfaces::srv::SetDirection_Response & msg)
  : msg_(msg)
  {}
  ::balance_hw_interfaces::srv::SetDirection_Response message(::balance_hw_interfaces::srv::SetDirection_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Response msg_;
};

class Init_SetDirection_Response_success
{
public:
  Init_SetDirection_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetDirection_Response_message success(::balance_hw_interfaces::srv::SetDirection_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_SetDirection_Response_message(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::balance_hw_interfaces::srv::SetDirection_Response>()
{
  return balance_hw_interfaces::srv::builder::Init_SetDirection_Response_success();
}

}  // namespace balance_hw_interfaces


namespace balance_hw_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetDirection_Event_response
{
public:
  explicit Init_SetDirection_Event_response(::balance_hw_interfaces::srv::SetDirection_Event & msg)
  : msg_(msg)
  {}
  ::balance_hw_interfaces::srv::SetDirection_Event response(::balance_hw_interfaces::srv::SetDirection_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Event msg_;
};

class Init_SetDirection_Event_request
{
public:
  explicit Init_SetDirection_Event_request(::balance_hw_interfaces::srv::SetDirection_Event & msg)
  : msg_(msg)
  {}
  Init_SetDirection_Event_response request(::balance_hw_interfaces::srv::SetDirection_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_SetDirection_Event_response(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Event msg_;
};

class Init_SetDirection_Event_info
{
public:
  Init_SetDirection_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SetDirection_Event_request info(::balance_hw_interfaces::srv::SetDirection_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_SetDirection_Event_request(msg_);
  }

private:
  ::balance_hw_interfaces::srv::SetDirection_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::balance_hw_interfaces::srv::SetDirection_Event>()
{
  return balance_hw_interfaces::srv::builder::Init_SetDirection_Event_info();
}

}  // namespace balance_hw_interfaces

#endif  // BALANCE_HW_INTERFACES__SRV__DETAIL__SET_DIRECTION__BUILDER_HPP_
