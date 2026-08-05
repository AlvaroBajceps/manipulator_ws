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
find_package(rclcpp_components REQUIRED)


add_library(manipulator_simple_component SHARED src/ManipulatorSimple.cpp)

rclcpp_components_register_node(
  manipulator_simple_component
  PLUGIN "ManipulatorSimple"
  EXECUTABLE manipulator_simple_node
)

ament_target_dependencies(manipulator_simple_component
  rclcpp
  std_msgs
  rex_interfaces
  rclcpp_components
)

target_include_directories(${PROJECT_NAME}_component PUBLIC
   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
   "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")


target_link_libraries(manipulator_simple_component
  ${cpp_typesupport_target}
  libVescCan::libVescCan
)

# Export
ament_export_dependencies(
  rosidl_default_runtime
  rex_interfaces
)

# Install
install(TARGETS manipulator_simple_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_package()
