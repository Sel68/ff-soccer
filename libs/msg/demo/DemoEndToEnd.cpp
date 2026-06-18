#include <iostream>

#include "EncodingDecoding.h"
#include "MessageInterface.h"
#include "SystemConstants.h"

int main() {
  // 1. Host prepares all the commands
  std::array<RobotCommandMsg, SystemConstants::num_robots + 1> robot_command_msg;

  for (int i = 1; i <= SystemConstants::num_robots; ++i) {
    robot_command_msg[i].id = i;
    robot_command_msg[i].vx = 1;
    robot_command_msg[i].vy = 1.222;
    robot_command_msg[i].w = 243.1223;
  }

  // 2. Host prepares a string

  // string: [len, .........., len, ......, len, ....]

  std::string host_msg;
  for (int i = 1; i <= SystemConstants::num_robots; ++i) {
    std::string cmd_i = SerializeRobotCommandMsg(robot_command_msg[i]);
    uint16_t len = static_cast<uint16_t>(cmd_i.size());

    host_msg.append(reinterpret_cast<const char*>(&len), sizeof(len));
    host_msg.append(cmd_i);
  }

  // 4. Host sends this to the base station
  std::string out_msg = host_msg;

  std::cout << "Out msg (hex): ";
  for (unsigned char c : out_msg) {
    std::cout << std::hex << (int)c << " ";
  }
  std::cout << std::dec << "\n";

  // 5. Base station receives the host msg and deserialzies it
  std::array<std::string, SystemConstants::num_robots + 1> rob_cmd_ser;
  size_t offset = 0;
  for (int i = 1; i <= SystemConstants::num_robots; ++i) {
    uint16_t len = 0;
    memcpy(&len, out_msg.data() + offset, sizeof(len));
    offset += sizeof(len);

    rob_cmd_ser[i] = out_msg.substr(offset, len);
    offset += len;
  }

  std::cout << "Hey\r\n";

  // 6. Base station sends each rob_cmd_ser to the relevant robot

  // 7. Let's say... Robot 2 receives this serialized command
  uint8_t device_id = 2;
  std::string rob_i = rob_cmd_ser[device_id];

  RobotCommandMsg rob_cmd;
  if (!DeserializeRobotCommandMsg(rob_i, rob_cmd)) {
    std::cout << "[Robot]: Failed to deserialize robot command message\r\n";
  }

  // 8. Robot uses this command
  std::cout << "Robot Command: " << rob_cmd.id << ", " << rob_cmd.vx << ", " << rob_cmd.vy << ", "
            << rob_cmd.w << std::endl;
}