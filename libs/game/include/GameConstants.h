#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include <glm/glm.hpp>

struct GameConstants {
  static constexpr float PLAYER_RADIUS = 18.5f;
  static constexpr float PLAYER_VELOCITY = 300.0f;
  static constexpr glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(150.0f, 150.0f);
  static constexpr float BALL_RADIUS = 12.5f;
  static constexpr float Stuckerror = 2.0f;

  static constexpr unsigned int SCREEN_WIDTH = 950;
  static constexpr unsigned int SCREEN_HEIGHT = 580;
};

#endif  // GAME_CONSTANTS_H