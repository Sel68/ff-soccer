#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <cstdint>

#include "SystemConstants.h"

struct NetworkConfig {
  // IPs (192.168.1.XYZ)
  static constexpr uint8_t base_station_ip = 100;
  static constexpr uint8_t host_ip = 101;

  // Ports
  static constexpr uint16_t host_listen_port = 8080;
  static constexpr uint16_t base_station_listen_port = 8081;

  // Message sizes (WIRED)
  static constexpr int udp_buffer_max_size = 1024;

  // Message sizees (WIRELESS)
  static constexpr uint8_t radio_msg_size = 255;
  static constexpr uint8_t radio_msg_header_size = 2;
  static constexpr uint8_t radio_msg_crc_size = 2;
  static constexpr uint8_t radio_msg_payload_size =
      radio_msg_size - (radio_msg_header_size + radio_msg_crc_size);  // 128 - (2 + 2) = 124

  // Timing
  static constexpr uint16_t network_cycle_time = 1000;
  static constexpr uint16_t beacon_time = 10;
  static constexpr uint16_t udp_time = 30;
  static constexpr uint16_t robot_command_time = 10;
  static constexpr uint16_t robot_response_time = 10;

  static_assert(beacon_time + udp_time +
                        (robot_command_time + robot_response_time) * SystemConstants::num_robots <=
                    network_cycle_time &&
                "Total network cycle time issue");

  static uint16_t GetBeaconStartTime(uint16_t cycle_start_time) { return cycle_start_time; }

  static uint16_t GetUdpStartTime(uint16_t cycle_start_time) {
    return GetBeaconStartTime(cycle_start_time) + beacon_time;
  }

  static uint16_t GetRobotCommandStartTime(uint16_t cycle_start_time, uint8_t robot_idx) {
    // 1: B + H
    // 2: B + H + R1
    // 6: B + H + R1 ... + R5
    return GetUdpStartTime(cycle_start_time) + udp_time + robot_command_time * (robot_idx - 1);
  }

  static uint16_t GetRobotResponseStartTime(uint16_t cycle_start_time, uint8_t robot_idx) {
    // 1: B + H + R1 ... R6
    // 2: 1 + R1
    // 3: ...
    // 6:   N +  B    U  +   C * 6   +   R * 5
    return GetRobotCommandStartTime(cycle_start_time, SystemConstants::num_robots + 1) +
           robot_response_time * (robot_idx - 1);
  }
};

#endif  // NETWORK_CONFIG_H
