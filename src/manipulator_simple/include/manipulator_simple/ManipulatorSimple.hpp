#pragma once

#include "rclcpp/rclcpp.hpp"
#include "rex_interfaces/msg/robotic_arm_control.hpp"
#include "rex_interfaces/msg/manipulator_control.hpp"
#include "rex_interfaces/msg/rover_status.hpp"
extern "C"
{
#include "libVescCan/VESC.h"
}

class ManipulatorSimple
{
public:
    ManipulatorSimple(rclcpp::Node::SharedPtr node);

private:
    void handleInput(rex_interfaces::msg::RoboticArmControl roboticArmControlMessage);
    void handleRoverStatus(rex_interfaces::msg::RoverStatus::ConstSharedPtr msg);

    void publishCommand();

    rclcpp::Node::SharedPtr mNode;

    const rex_interfaces::msg::ManipulatorControl mStopCommand;
    rex_interfaces::msg::ManipulatorControl mLastCommandMessage;

    rclcpp::Subscription<rex_interfaces::msg::RoboticArmControl>::SharedPtr mSubMqtt;
    rclcpp::Subscription<rex_interfaces::msg::RoverStatus>::SharedPtr mSubStatus;
    rclcpp::Publisher<rex_interfaces::msg::ManipulatorControl>::SharedPtr mPubCan;

    rclcpp::TimerBase::SharedPtr mResenderTimer;
    rex_interfaces::msg::RoverStatus::ConstSharedPtr mLastRoverStatus;
};