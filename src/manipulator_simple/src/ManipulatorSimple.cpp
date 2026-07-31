#include "manipulator_simple/ManipulatorSimple.hpp"

namespace
{
    rex_interfaces::msg::ManipulatorControl prepareStopCommand()
    {
        rex_interfaces::msg::ManipulatorControl stopCommand{};

        stopCommand.axes[0].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.axes[1].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.axes[2].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.axes[3].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.axes[4].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.axes[5].command_id = VESC_COMMAND_SET_DUTY;
        stopCommand.gripper.command_id = VESC_COMMAND_SET_DUTY;

        stopCommand.axes[0].set_value = 0.0f;
        stopCommand.axes[1].set_value = 0.0f;
        stopCommand.axes[2].set_value = 0.0f;
        stopCommand.axes[3].set_value = 0.0f;
        stopCommand.axes[4].set_value = 0.0f;
        stopCommand.axes[5].set_value = 0.0f;
        stopCommand.gripper.set_value = 0.0f;

        return stopCommand;
    }
}

ManipulatorSimple::ManipulatorSimple(rclcpp::Node::SharedPtr node)
    : mNode(node),
      mStopCommand(prepareStopCommand()),
      mLastRoverStatus(std::make_shared<rex_interfaces::msg::RoverStatus>())
{
    mSubMqtt = mNode->create_subscription<rex_interfaces::msg::RoboticArmControl>("/MQTT/RoboticArmControl", 100, std::bind(&ManipulatorSimple::handleInput, this, std::placeholders::_1));
    mSubStatus = mNode->create_subscription<rex_interfaces::msg::RoverStatus>(
        "/MQTT/RoverStatus",
        rclcpp::QoS(10), std::bind(&ManipulatorSimple::handleRoverStatus, this, std::placeholders::_1));
    mPubCan = mNode->create_publisher<rex_interfaces::msg::ManipulatorControl>("/CAN/TX/manipulator_ctl", 100);

    mResenderTimer = mNode->create_timer(std::chrono::milliseconds(50), std::bind(&ManipulatorSimple::publishCommand, this));

    mLastCommandMessage = mStopCommand;
}

void ManipulatorSimple::handleInput(rex_interfaces::msg::RoboticArmControl roboticArmControlMessage)
{
    // The assumption is that this package runs instead of the new inverse kinematics stack

    if (roboticArmControlMessage.action_type != rex_interfaces::msg::RoboticArmControl::ACTION_FORWARD_KIN)
    {
        mLastCommandMessage = mStopCommand;
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
    // This package is not responsible for stopping the arm after disabling ROBOTIC_ARM in RoverStatus
    // See can_bridge instead
    mLastRoverStatus = msg;
}

void ManipulatorSimple::publishCommand()
{
    if ((mLastRoverStatus->control_mode & rex_interfaces::msg::RoverStatus::CONTROL_MODE_ROBOTIC_ARM) == 0)
    {
        // Reset mLastCommandMessage so that the arm won't move after it's turned on again.
        mLastCommandMessage = mStopCommand;
        return;
    }

    mLastCommandMessage.header.stamp = rclcpp::Time();
    mPubCan->publish(mLastCommandMessage);
}