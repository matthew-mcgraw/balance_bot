// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from balance_hw_interfaces:srv/SetPwm.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "balance_hw_interfaces/srv/detail/set_pwm__functions.h"
#include "balance_hw_interfaces/srv/detail/set_pwm__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetPwm_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetPwm_Request_type_support_ids_t;

static const _SetPwm_Request_type_support_ids_t _SetPwm_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetPwm_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetPwm_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetPwm_Request_type_support_symbol_names_t _SetPwm_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, balance_hw_interfaces, srv, SetPwm_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, balance_hw_interfaces, srv, SetPwm_Request)),
  }
};

typedef struct _SetPwm_Request_type_support_data_t
{
  void * data[2];
} _SetPwm_Request_type_support_data_t;

static _SetPwm_Request_type_support_data_t _SetPwm_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetPwm_Request_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetPwm_Request_message_typesupport_ids.typesupport_identifier[0],
  &_SetPwm_Request_message_typesupport_symbol_names.symbol_name[0],
  &_SetPwm_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetPwm_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetPwm_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetPwm_Request__get_type_hash,
  &balance_hw_interfaces__srv__SetPwm_Request__get_type_description,
  &balance_hw_interfaces__srv__SetPwm_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace balance_hw_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Request>()
{
  return &::balance_hw_interfaces::srv::rosidl_typesupport_cpp::SetPwm_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, balance_hw_interfaces, srv, SetPwm_Request)() {
  return get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_pwm__functions.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_pwm__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetPwm_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetPwm_Response_type_support_ids_t;

static const _SetPwm_Response_type_support_ids_t _SetPwm_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetPwm_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetPwm_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetPwm_Response_type_support_symbol_names_t _SetPwm_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, balance_hw_interfaces, srv, SetPwm_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, balance_hw_interfaces, srv, SetPwm_Response)),
  }
};

typedef struct _SetPwm_Response_type_support_data_t
{
  void * data[2];
} _SetPwm_Response_type_support_data_t;

static _SetPwm_Response_type_support_data_t _SetPwm_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetPwm_Response_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetPwm_Response_message_typesupport_ids.typesupport_identifier[0],
  &_SetPwm_Response_message_typesupport_symbol_names.symbol_name[0],
  &_SetPwm_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetPwm_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetPwm_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetPwm_Response__get_type_hash,
  &balance_hw_interfaces__srv__SetPwm_Response__get_type_description,
  &balance_hw_interfaces__srv__SetPwm_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace balance_hw_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Response>()
{
  return &::balance_hw_interfaces::srv::rosidl_typesupport_cpp::SetPwm_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, balance_hw_interfaces, srv, SetPwm_Response)() {
  return get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_pwm__functions.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_pwm__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetPwm_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetPwm_Event_type_support_ids_t;

static const _SetPwm_Event_type_support_ids_t _SetPwm_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetPwm_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetPwm_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetPwm_Event_type_support_symbol_names_t _SetPwm_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, balance_hw_interfaces, srv, SetPwm_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, balance_hw_interfaces, srv, SetPwm_Event)),
  }
};

typedef struct _SetPwm_Event_type_support_data_t
{
  void * data[2];
} _SetPwm_Event_type_support_data_t;

static _SetPwm_Event_type_support_data_t _SetPwm_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetPwm_Event_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetPwm_Event_message_typesupport_ids.typesupport_identifier[0],
  &_SetPwm_Event_message_typesupport_symbol_names.symbol_name[0],
  &_SetPwm_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetPwm_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetPwm_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetPwm_Event__get_type_hash,
  &balance_hw_interfaces__srv__SetPwm_Event__get_type_description,
  &balance_hw_interfaces__srv__SetPwm_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace balance_hw_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Event>()
{
  return &::balance_hw_interfaces::srv::rosidl_typesupport_cpp::SetPwm_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, balance_hw_interfaces, srv, SetPwm_Event)() {
  return get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_pwm__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _SetPwm_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetPwm_type_support_ids_t;

static const _SetPwm_type_support_ids_t _SetPwm_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _SetPwm_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetPwm_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetPwm_type_support_symbol_names_t _SetPwm_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, balance_hw_interfaces, srv, SetPwm)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, balance_hw_interfaces, srv, SetPwm)),
  }
};

typedef struct _SetPwm_type_support_data_t
{
  void * data[2];
} _SetPwm_type_support_data_t;

static _SetPwm_type_support_data_t _SetPwm_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetPwm_service_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetPwm_service_typesupport_ids.typesupport_identifier[0],
  &_SetPwm_service_typesupport_symbol_names.symbol_name[0],
  &_SetPwm_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t SetPwm_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetPwm_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<balance_hw_interfaces::srv::SetPwm_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<balance_hw_interfaces::srv::SetPwm>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<balance_hw_interfaces::srv::SetPwm>,
  &balance_hw_interfaces__srv__SetPwm__get_type_hash,
  &balance_hw_interfaces__srv__SetPwm__get_type_description,
  &balance_hw_interfaces__srv__SetPwm__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace balance_hw_interfaces

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<balance_hw_interfaces::srv::SetPwm>()
{
  return &::balance_hw_interfaces::srv::rosidl_typesupport_cpp::SetPwm_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, balance_hw_interfaces, srv, SetPwm)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<balance_hw_interfaces::srv::SetPwm>();
}

#ifdef __cplusplus
}
#endif
