#ifndef SYSTEM_CONSTANTS
#define SYSTEM_CONSTANTS

#include <array>
#include <cstdint>
#include <utility>

struct SystemConstants {
  static constexpr uint8_t num_robots = 6;

  static_assert(num_robots >= 1 && num_robots <= 6 && "Check [SystemConstants]: num_robots\r\n");
  
  static constexpr unsigned int SCREEN_WIDTH = 950;
  static constexpr unsigned int SCREEN_HEIGHT = 580;
};

#endif  // SYSTEM_CONSTANTS
