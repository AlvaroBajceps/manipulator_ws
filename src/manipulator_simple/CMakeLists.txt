cmake_minimum_required(VERSION 3.8)
project(manipulator_simple)

# C++ version
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rex_interfaces REQUIRED)
find_package(libVescCan REQUIRED)

# Message generation
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/ManipulatorControl.msg"
  DEPENDENCIES std_msgs rex_interfaces
)

# Executable
add_executable(manipulator_simple_node src/main.cpp)

ament_target_dependencies(manipulator_simple_node
  rclcpp
  std_msgs
  rex_interfaces
)

target_include_directories(${PROJECT_NAME}_node PUBLIC
   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
   "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# Link generated message typesupport
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

target_link_libraries(manipulator_simple_node
  ${cpp_typesupport_target}
  libVescCan::libVescCan
)

# Export
ament_export_dependencies(
  rosidl_default_runtime
  rex_interfaces 
)

# Install
install(TARGETS
  manipulator_simple_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
