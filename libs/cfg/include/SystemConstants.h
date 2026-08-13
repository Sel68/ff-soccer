#ifndef SYSTEM_CONSTANTS
#define SYSTEM_CONSTANTS

#include <array>
#include <cstdint>
#include <utility>

struct SystemConstants {
  static constexpr uint8_t num_robots = 6;
  static constexpr uint8_t num_drive_motors = 4;
  static constexpr uint8_t num_motors = num_drive_motors + 1;
  static constexpr uint8_t num_wheels = 4;

  static_assert(num_robots >= 1 && num_robots <= 6 && "Check [SystemConstants]: num_robots\r\n");
  static_assert(num_motors == 5 && "Check [SystemConstants]: num_wheels\r\n");

  static constexpr unsigned int screen_width = 950;
  static constexpr unsigned int screen_height = 580;
};

#endif  // SYSTEM_CONSTANTS
