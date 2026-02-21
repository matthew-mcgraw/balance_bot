// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from balance_hw_interfaces:srv/SetPwm.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "balance_hw_interfaces/srv/set_pwm.h"


#ifndef BALANCE_HW_INTERFACES__SRV__DETAIL__SET_PWM__STRUCT_H_
#define BALANCE_HW_INTERFACES__SRV__DETAIL__SET_PWM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/SetPwm in the package balance_hw_interfaces.
typedef struct balance_hw_interfaces__srv__SetPwm_Request
{
  /// 0...1000 finer resolution than 0.100
  uint16_t pwm_a;
  uint16_t pwm_b;
} balance_hw_interfaces__srv__SetPwm_Request;

// Struct for a sequence of balance_hw_interfaces__srv__SetPwm_Request.
typedef struct balance_hw_interfaces__srv__SetPwm_Request__Sequence
{
  balance_hw_interfaces__srv__SetPwm_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} balance_hw_interfaces__srv__SetPwm_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/SetPwm in the package balance_hw_interfaces.
typedef struct balance_hw_interfaces__srv__SetPwm_Response
{
  bool success;
  rosidl_runtime_c__String message;
} balance_hw_interfaces__srv__SetPwm_Response;

// Struct for a sequence of balance_hw_interfaces__srv__SetPwm_Response.
typedef struct balance_hw_interfaces__srv__SetPwm_Response__Sequence
{
  balance_hw_interfaces__srv__SetPwm_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} balance_hw_interfaces__srv__SetPwm_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  balance_hw_interfaces__srv__SetPwm_Event__request__MAX_SIZE = 1
};
// response
enum
{
  balance_hw_interfaces__srv__SetPwm_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/SetPwm in the package balance_hw_interfaces.
typedef struct balance_hw_interfaces__srv__SetPwm_Event
{
  service_msgs__msg__ServiceEventInfo info;
  balance_hw_interfaces__srv__SetPwm_Request__Sequence request;
  balance_hw_interfaces__srv__SetPwm_Response__Sequence response;
} balance_hw_interfaces__srv__SetPwm_Event;

// Struct for a sequence of balance_hw_interfaces__srv__SetPwm_Event.
typedef struct balance_hw_interfaces__srv__SetPwm_Event__Sequence
{
  balance_hw_interfaces__srv__SetPwm_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} balance_hw_interfaces__srv__SetPwm_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // BALANCE_HW_INTERFACES__SRV__DETAIL__SET_PWM__STRUCT_H_
