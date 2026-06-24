#include "manipulator_simple/ManipulatorSimple.hpp"

ManipulatorSimple::ManipulatorSimple(rclcpp::Node::SharedPtr node) : mNode(node), mLastRoverStatus(std::make_shared<rex_interfaces::msg::RoverStatus>())
{
    mSubMqtt = mNode->create_subscription<rex_interfaces::msg::RoboticArmControl>("/MQTT/RoboticArmControl", 100, std::bind(&ManipulatorSimple::handleInput, this, std::placeholders::_1));
    mSubStatus = mNode->create_subscription<rex_interfaces::msg::RoverStatus>(
        "/MQTT/RoverStatus",
        rclcpp::QoS(10), std::bind(&ManipulatorSimple::handleRoverStatus, this, std::placeholders::_1));
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

    mLastCommandMessage = rex_interfaces::msg::ManipulatorControl(mLastCommandMessage);
    mLastCommandMessage.header.stamp.sec = 0;
}

void ManipulatorSimple::handleInput(rex_interfaces::msg::RoboticArmControl roboticArmControlMessage)
{
    if (roboticArmControlMessage.action_type != rex_interfaces::msg::RoboticArmControl::ACTION_FORWARD_KIN)
    {
        mLastCommandMessage = rex_interfaces::msg::ManipulatorControl{};
        if (roboticArmControlMessage.action_type == rex_interfaces::msg::RoboticArmControl::ACTION_STOP)
            publishCommand();
        return;
    }

    mLastCommandMessage.axes[0].set_value = roboticArmControlMessage.joint_positions[0];
    mLastCommandMessage.axes[1].set_value = roboticArmControlMessage.joint_positions[1];
    mLastCommandMessage.axes[2].set_value = roboticArmControlMessage.joint_positions[2];
    mLastCommandMessage.axes[3].set_value = roboticArmControlMessage.joint_positions[3];
    mLastCommandMessage.axes[4].set_value = roboticArmControlMessage.joint_positions[4];
    mLastCommandMessage.axes[5].set_value = roboticArmControlMessage.joint_positions[5];
    mLastCommandMessage.gripper.set_value = roboticArmControlMessage.gripper;
}

void ManipulatorSimple::handleRoverStatus(rex_interfaces::msg::RoverStatus::ConstSharedPtr msg)
{
    mLastRoverStatus = msg;
    publishCommand();
}

void ManipulatorSimple::publishCommand()
{
    rex_interfaces::msg::ManipulatorControl messageToSend;

    if (mLastRoverStatus->communication_state == VESC_STATUS_10_COMMUNICATIONSTATE_OPENED && mLastRoverStatus->control_mode != VESC_STATUS_10_CONTROLMODE_ESTOP)
    {
        messageToSend = rex_interfaces::msg::ManipulatorControl(mLastCommandMessage);
    }
    else
    {
        messageToSend = rex_interfaces::msg::ManipulatorControl();
    }
    messageToSend.header.stamp = rclcpp::Time();
    mPubCan->publish(messageToSend);
}