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
find_package(rex_interfaces REQUIRED)
find_package(libVescCan REQUIRED)


# Executable
add_executable(manipulator_simple_node 
  src/main.cpp
  src/ManipulatorSimple.cpp
)

ament_target_dependencies(manipulator_simple_node
  rclcpp
  std_msgs
  rex_interfaces
)

target_include_directories(${PROJECT_NAME}_node PUBLIC
   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
   "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")


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
