// generated from rosidl_typesupport_c/resource/idl__type_support.cpp.em
// with input from balance_hw_interfaces:srv/SetDirection.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "balance_hw_interfaces/srv/detail/set_direction__struct.h"
#include "balance_hw_interfaces/srv/detail/set_direction__type_support.h"
#include "balance_hw_interfaces/srv/detail/set_direction__functions.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _SetDirection_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetDirection_Request_type_support_ids_t;

static const _SetDirection_Request_type_support_ids_t _SetDirection_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _SetDirection_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetDirection_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetDirection_Request_type_support_symbol_names_t _SetDirection_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, balance_hw_interfaces, srv, SetDirection_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, balance_hw_interfaces, srv, SetDirection_Request)),
  }
};

typedef struct _SetDirection_Request_type_support_data_t
{
  void * data[2];
} _SetDirection_Request_type_support_data_t;

static _SetDirection_Request_type_support_data_t _SetDirection_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetDirection_Request_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetDirection_Request_message_typesupport_ids.typesupport_identifier[0],
  &_SetDirection_Request_message_typesupport_symbol_names.symbol_name[0],
  &_SetDirection_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetDirection_Request_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetDirection_Request_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetDirection_Request__get_type_hash,
  &balance_hw_interfaces__srv__SetDirection_Request__get_type_description,
  &balance_hw_interfaces__srv__SetDirection_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace balance_hw_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, balance_hw_interfaces, srv, SetDirection_Request)() {
  return &::balance_hw_interfaces::srv::rosidl_typesupport_c::SetDirection_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__type_support.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _SetDirection_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetDirection_Response_type_support_ids_t;

static const _SetDirection_Response_type_support_ids_t _SetDirection_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _SetDirection_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetDirection_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetDirection_Response_type_support_symbol_names_t _SetDirection_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, balance_hw_interfaces, srv, SetDirection_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, balance_hw_interfaces, srv, SetDirection_Response)),
  }
};

typedef struct _SetDirection_Response_type_support_data_t
{
  void * data[2];
} _SetDirection_Response_type_support_data_t;

static _SetDirection_Response_type_support_data_t _SetDirection_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetDirection_Response_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetDirection_Response_message_typesupport_ids.typesupport_identifier[0],
  &_SetDirection_Response_message_typesupport_symbol_names.symbol_name[0],
  &_SetDirection_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetDirection_Response_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetDirection_Response_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetDirection_Response__get_type_hash,
  &balance_hw_interfaces__srv__SetDirection_Response__get_type_description,
  &balance_hw_interfaces__srv__SetDirection_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace balance_hw_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, balance_hw_interfaces, srv, SetDirection_Response)() {
  return &::balance_hw_interfaces::srv::rosidl_typesupport_c::SetDirection_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__type_support.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{

typedef struct _SetDirection_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetDirection_Event_type_support_ids_t;

static const _SetDirection_Event_type_support_ids_t _SetDirection_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _SetDirection_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetDirection_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetDirection_Event_type_support_symbol_names_t _SetDirection_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, balance_hw_interfaces, srv, SetDirection_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, balance_hw_interfaces, srv, SetDirection_Event)),
  }
};

typedef struct _SetDirection_Event_type_support_data_t
{
  void * data[2];
} _SetDirection_Event_type_support_data_t;

static _SetDirection_Event_type_support_data_t _SetDirection_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetDirection_Event_message_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetDirection_Event_message_typesupport_ids.typesupport_identifier[0],
  &_SetDirection_Event_message_typesupport_symbol_names.symbol_name[0],
  &_SetDirection_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t SetDirection_Event_message_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetDirection_Event_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &balance_hw_interfaces__srv__SetDirection_Event__get_type_hash,
  &balance_hw_interfaces__srv__SetDirection_Event__get_type_description,
  &balance_hw_interfaces__srv__SetDirection_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace balance_hw_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, balance_hw_interfaces, srv, SetDirection_Event)() {
  return &::balance_hw_interfaces::srv::rosidl_typesupport_c::SetDirection_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "balance_hw_interfaces/srv/detail/set_direction__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/service_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
#include "service_msgs/msg/service_event_info.h"
#include "builtin_interfaces/msg/time.h"

namespace balance_hw_interfaces
{

namespace srv
{

namespace rosidl_typesupport_c
{
typedef struct _SetDirection_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _SetDirection_type_support_ids_t;

static const _SetDirection_type_support_ids_t _SetDirection_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
  }
};

typedef struct _SetDirection_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _SetDirection_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _SetDirection_type_support_symbol_names_t _SetDirection_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, balance_hw_interfaces, srv, SetDirection)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, balance_hw_interfaces, srv, SetDirection)),
  }
};

typedef struct _SetDirection_type_support_data_t
{
  void * data[2];
} _SetDirection_type_support_data_t;

static _SetDirection_type_support_data_t _SetDirection_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _SetDirection_service_typesupport_map = {
  2,
  "balance_hw_interfaces",
  &_SetDirection_service_typesupport_ids.typesupport_identifier[0],
  &_SetDirection_service_typesupport_symbol_names.symbol_name[0],
  &_SetDirection_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t SetDirection_service_type_support_handle = {
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_SetDirection_service_typesupport_map),
  rosidl_typesupport_c__get_service_typesupport_handle_function,
  &SetDirection_Request_message_type_support_handle,
  &SetDirection_Response_message_type_support_handle,
  &SetDirection_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    balance_hw_interfaces,
    srv,
    SetDirection
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    balance_hw_interfaces,
    srv,
    SetDirection
  ),
  &balance_hw_interfaces__srv__SetDirection__get_type_hash,
  &balance_hw_interfaces__srv__SetDirection__get_type_description,
  &balance_hw_interfaces__srv__SetDirection__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace srv

}  // namespace balance_hw_interfaces

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_c, balance_hw_interfaces, srv, SetDirection)() {
  return &::balance_hw_interfaces::srv::rosidl_typesupport_c::SetDirection_service_type_support_handle;
}

#ifdef __cplusplus
}
#endif
