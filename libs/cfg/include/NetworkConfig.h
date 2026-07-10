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
  static constexpr uint8_t radio_msg_size = 60;
  static constexpr uint8_t radio_msg_header_size = 2;
  static constexpr uint8_t radio_msg_crc_size = 2;
  static constexpr uint8_t radio_msg_payload_size =
      radio_msg_size - (radio_msg_header_size + radio_msg_crc_size);  // 255 - (2 + 2) = 251

  // Event flags
  static constexpr uint32_t radio_rx_irq = 1u << 0;
  static constexpr uint32_t radio_tx_irq = 1u << 1;
  static constexpr uint32_t radio_timeout_irq = 1u << 2;
};

struct NetworkTime {
  // Timing
  static constexpr uint32_t ticks_rate = 4;
  static constexpr uint32_t to_us = 1000;
  static constexpr uint32_t network_cycle_time = 40 * to_us;  // 40 is fine
  static constexpr uint32_t beacon_time = (2 * to_us);
  static constexpr uint32_t udp_time = 5 * to_us;
  static constexpr uint32_t robot_command_time = (2 * to_us);
  static constexpr uint32_t robot_response_time = (2 * to_us);
  static constexpr uint32_t receive_offset = 0 * to_us;

  static constexpr uint32_t spi_timeout = 5 * to_us;

  static constexpr uint32_t predicted_beacon_travel_time = 10 * to_us;
  static constexpr uint32_t time_offset_diff_thresh = 5 * to_us;

  static constexpr uint32_t sx1280_timeout_delay = 320;  // Microseconds

  static_assert(beacon_time + udp_time +
                        (robot_command_time + robot_response_time) * SystemConstants::num_robots <=
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
    return GetRobotCommandStartTime(cycle_start_time, SystemConstants::num_robots + 1) +
           robot_response_time * (robot_idx - 1);
  }
};

#endif  // NETWORK_CONFIG_H
