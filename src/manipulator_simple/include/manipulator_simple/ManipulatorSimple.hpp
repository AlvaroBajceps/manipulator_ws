#pragma once

#include "rclcpp/rclcpp.hpp"
#include "rex_interfaces/msg/manipulator_mqtt_message.hpp"
#include "rex_interfaces/msg/manipulator_control.hpp"
extern "C"
{
#include "libVescCan/VESC.h"
}

class ManipulatorSimple
{
public:
    ManipulatorSimple(rclcpp::Node::SharedPtr node);
private:
    void handleInput(rex_interfaces::msg::ManipulatorMqttMessage manipulatorControlMessage);

    void publishCommand();

    rclcpp::Node::SharedPtr mNode;

    rex_interfaces::msg::ManipulatorControl mLastCommandMessage;

    rclcpp::Subscription<rex_interfaces::msg::ManipulatorMqttMessage>::SharedPtr mSubMqtt;
    rclcpp::Publisher<rex_interfaces::msg::ManipulatorControl>::SharedPtr mPubCan;

    rclcpp::TimerBase::SharedPtr mResenderTimer;
};