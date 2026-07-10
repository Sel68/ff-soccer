#ifndef ENCODING_DECODING_H
#define ENCODING_DECODING_H

#include "MessageInterface.h"
#include "SystemConstants.h"

using RobotCommands =
    std::array<RobotCommandMsg, SystemConstants::num_robots + 1>;

/*
  Host: Serializes host msg, Serializes soccer msg
  Robot: Deserializes soccer msg, deserializes host msg
  Robot: Serializes robot msg, Serializes soccer msg
  Host: Deserializes soccer msg, deserializes robot msg
*/

/******** FOR MCUs ********/
#ifdef BUILD_ON_STM

#include <string_view>

// Robot
std::string SerializeRobotResponseMsg(
    const RobotResponseMsg &robot_response_msg = RobotResponseMsg_init_zero);
bool DeserializeRobotCommandMsg(const std::string_view &payload,
                                RobotCommandMsg &robot_command_msg);

#else
/******** FOR LAPTOP ******/

// Host
std::string SerializeRobotCommandMsg(const RobotCommandMsg &robot_command_msg);

// Robot
bool DeserializeRobotResponseMsg(const std::string &payload,
                                 RobotResponseMsg &robot_response_msg);
bool DeserializeRobotCommandMsg(const std::string &payload,
                                RobotCommandMsg &robot_command_msg);

#endif

#endif // ENCODING_DECODING_H
