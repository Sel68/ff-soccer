#ifndef SYSTEM_CONSTANTS
#define SYSTEM_CONSTANTS

#include <array>
#include <cstdint>
#include <utility>

struct SystemConstants {
  static constexpr uint8_t num_robots = 1;

  static constexpr float PLAYER_RADIUS = 18.5f;
  static constexpr float PLAYER_VELOCITY = 300.0f;
  static constexpr std::pair<float, float> INITIAL_BALL_VELOCITY =
      std::pair<float, float>(150.0f, 150.0f);
  static constexpr float BALL_RADIUS = 12.5f;
  static constexpr float Stuckerror = 2.0f;

  static constexpr unsigned int SCREEN_WIDTH = 950;
  static constexpr unsigned int SCREEN_HEIGHT = 580;

  static constexpr float OBSTACLE_TOLERANCE = 30.0f;

  // Embedded
};

#endif  // SYSTEM_CONSTANTS