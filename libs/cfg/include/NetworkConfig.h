#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <cstdint>

#include "SystemConstants.h"

struct NetworkConfig {
  // IPs (192.168.1.XYZ)
  static constexpr uint8_t base_station_ip[4] = {192, 168, 1, 100};
  static constexpr uint8_t host_ip[4] = {192, 168, 1, 101};

  // Ports
  static constexpr uint32_t host_port = 8080;
  static constexpr uint32_t base_station_port = 8081;

  // Message sizes (WIRED)
  static constexpr int udp_buffer_max_size = 1600;

  // Message sizees (WIRELESS)
  static constexpr uint8_t radio_msg_size = 255;
  static constexpr uint8_t radio_msg_header_size = 2;
  static constexpr uint8_t radio_msg_crc_size = 2;
  static constexpr uint8_t radio_msg_payload_size =
      radio_msg_size - (radio_msg_header_size + radio_msg_crc_size);  // 255 - (2 + 2) = 251
};

struct NetworkTime {
  // Timing
  static constexpr uint32_t network_cycle_time = 1000;
  static constexpr uint32_t beacon_time = 30;
  static constexpr uint32_t udp_time = 30;
  static constexpr uint32_t robot_command_time = 30;
  static constexpr uint32_t robot_response_time = 30;
  static constexpr uint32_t receive_offset = 10;

  static_assert(beacon_time + udp_time +
                        (robot_command_time + robot_response_time) * SystemConstants::NUM_ROBOTS <=
                    network_cycle_time &&
                "Total network cycle time issue");

  static uint32_t GetBeaconStartTime(uint32_t cycle_start_time) { return cycle_start_time; }

  static uint32_t GetUdpStartTime(uint32_t cycle_start_time) {
    return GetBeaconStartTime(cycle_start_time) + beacon_time;
  }

  static uint32_t GetRobotCommandStartTime(uint32_t cycle_start_time, uint8_t robot_idx = 1) {
    // 1: B + H
    // 2: B + H + R1
    // 6: B + H + R1 ... + R5
    return GetUdpStartTime(cycle_start_time) + udp_time + robot_command_time * (robot_idx - 1);
  }

  static uint32_t GetRobotResponseStartTime(uint32_t cycle_start_time, uint8_t robot_idx = 1) {
    // 1: B + H + R1 ... R6
    // 2: 1 + R1
    // 3: ...
    // 6:   N +  B    U  +   C * 6   +   R * 5
    return GetRobotCommandStartTime(cycle_start_time, SystemConstants::NUM_ROBOTS + 1) +
           robot_response_time * (robot_idx - 1);
  }
};

#endif  // NETWORK_CONFIG_H
