#ifndef GAME_CONSTANTS_H

struct GameConstants {
  static constexpr float GameConstants::PLAYER_RADIUS = 18.5f;
  static constexpr float GameConstants::PLAYER_VELOCITY = 300.0f;
  static constexpr glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(150.0f, 150.0f);
  static constexpr float BALL_RADIUS = 12.5f;
};

#endif  // GAME_CONSTANTS_H