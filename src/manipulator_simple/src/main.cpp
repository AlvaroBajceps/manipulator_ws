#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "manipulator_simple/ManipulatorSimple.hpp"


int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
	auto n = rclcpp::Node::make_shared("manipulator_simple");

  printf("I am alive. Yippie!\n");

  ManipulatorSimple manipulator(n);
  
  rclcpp::spin(n);

  printf("I am ded. Ble\n");
  return 0;
}
