#include "rclcpp/rclcpp.hpp"
#include "manipulator_simple/msg/manipulator_control.hpp"
#include "rex_interfaces/msg/manipulator_control.hpp"
extern "C"
{
#include "libVescCan/VESC.h"
}

class ManipulatorSimple
{
public:
    ManipulatorSimple(rclcpp::Node::SharedPtr node) : mNode(node)
    {
        mSubMqtt = mNode->create_subscription<manipulator_simple::msg::ManipulatorControl>("/MQTT/ManipulatorControl", 100, std::bind(&ManipulatorSimple::handleInput, this, std::placeholders::_1));
        mPubCan = mNode->create_publisher<rex_interfaces::msg::ManipulatorControl>("/CAN/TX/manipulator_ctl", 100);
        
        mLastCommandMessage = rex_interfaces::msg::ManipulatorControl();

        mResenderTimer = mNode->create_timer(std::chrono::milliseconds(50), std::bind(&ManipulatorSimple::publishCommand, this));

        mLastCommandMessage.axes[0].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.axes[1].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.axes[2].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.axes[3].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.axes[4].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.axes[5].command_id = VESC_COMMAND_SET_DUTY;
        mLastCommandMessage.gripper.command_id = VESC_COMMAND_SET_DUTY;

        mLastCommandMessage.axes[0].set_value = 0.0f;
        mLastCommandMessage.axes[1].set_value = 0.0f;
        mLastCommandMessage.axes[2].set_value = 0.0f;
        mLastCommandMessage.axes[3].set_value = 0.0f;
        mLastCommandMessage.axes[4].set_value = 0.0f;
        mLastCommandMessage.axes[5].set_value = 0.0f;
        mLastCommandMessage.gripper.set_value = 0.0f;
    }
private:
    void handleInput(manipulator_simple::msg::ManipulatorControl manipulatorControlMessage)
    {
        mLastCommandMessage.axes[0].set_value = manipulatorControlMessage.axis1;
        mLastCommandMessage.axes[1].set_value = manipulatorControlMessage.axis2;
        mLastCommandMessage.axes[2].set_value = manipulatorControlMessage.axis3;
        mLastCommandMessage.axes[3].set_value = manipulatorControlMessage.axis4;
        mLastCommandMessage.axes[4].set_value = manipulatorControlMessage.axis5;
        mLastCommandMessage.axes[5].set_value = manipulatorControlMessage.axis6;
        mLastCommandMessage.gripper.set_value = manipulatorControlMessage.gripper;
    }

    void publishCommand()
    {
        mPubCan->publish(mLastCommandMessage);
    }

    rclcpp::Node::SharedPtr mNode;

    rex_interfaces::msg::ManipulatorControl mLastCommandMessage;

    rclcpp::Subscription<manipulator_simple::msg::ManipulatorControl>::SharedPtr mSubMqtt;
    rclcpp::Publisher<rex_interfaces::msg::ManipulatorControl>::SharedPtr mPubCan;

    rclcpp::TimerBase::SharedPtr mResenderTimer;
};