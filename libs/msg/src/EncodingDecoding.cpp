#include "EncodingDecoding.h"

#include "Logger.h"
#include "SystemConstants.h"

#define PROTOBUF_BUFFER_SIZE 256
static uint8_t inner_buffer[PROTOBUF_BUFFER_SIZE];
static uint8_t outer_buffer[PROTOBUF_BUFFER_SIZE];

/***************** MCUs ***************/
#ifdef BUILD_ON_STM

std::string SerializeRobotResponseMsg(const RobotResponseMsg& robot_response_msg) {
  pb_ostream_t stream = pb_ostream_from_buffer(inner_buffer, sizeof(inner_buffer));

  if (!pb_encode(&stream, RobotResponseMsg_fields, &robot_response_msg)) {
    debug::Log() << "[SerializeRobotResponseMsg]: pb_encode failed\r\n";
    return {};
  }

  return std::string(reinterpret_cast<const char*>(inner_buffer), stream.bytes_written);
}

bool DeserializeRobotCommandMsg(const std::string_view& payload,
                                RobotCommandMsg& robot_command_msg) {
  robot_command_msg = RobotCommandMsg_init_zero;
  pb_istream_t stream =
      pb_istream_from_buffer(reinterpret_cast<const uint8_t*>(payload.data()), payload.size());
  if (!pb_decode(&stream, RobotCommandMsg_fields, &robot_command_msg)) {
    debug::Log() << "[DeserializeRobotCommandMsg]: pb_decode failed\r\n"
                     << PB_GET_ERROR(&stream)
                 << "\r\n";
    return false;
  }
  return true;
}

#else /***************** LAPTOP *************/

bool DeserializeRobotCommandMsg(const std::string& payload, RobotCommandMsg& robot_command_msg) {
  robot_command_msg = RobotCommandMsg_init_zero;
  pb_istream_t stream =
      pb_istream_from_buffer(reinterpret_cast<const uint8_t*>(payload.data()), payload.size());
  if (!pb_decode(&stream, RobotCommandMsg_fields, &robot_command_msg)) {
    debug::Log() << "[DeserializeRobotCommandMsg]: pb_decode failed\r\n";
    return false;
  }
  return true;
}

std::string SerializeRobotCommandMsg(const RobotCommandMsg& robot_command_msg) {
  uint8_t buffer[PROTOBUF_BUFFER_SIZE];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  if (!pb_encode(&stream, RobotCommandMsg_fields, &robot_command_msg)) {
    debug::Log() << "[SerializeRobotCommandMsg]: pb_encode failed\r\n";
    return "";
  }
  return std::string(reinterpret_cast<char*>(buffer), stream.bytes_written);
}

bool DeserializeRobotResponseMsg(const std::string& payload,
                                 RobotResponseMsg& robot_response_msg) {
  robot_response_msg = RobotResponseMsg_init_zero;
  pb_istream_t stream =
      pb_istream_from_buffer(reinterpret_cast<const uint8_t*>(payload.data()), payload.size());
  if (!pb_decode(&stream, RobotResponseMsg_fields, &robot_response_msg)) {
    debug::Log() << "[DeserializeRobotResponseMsg]: pb_decode failed\r\n";
    return false;
  }
  return true;
}

#endif
