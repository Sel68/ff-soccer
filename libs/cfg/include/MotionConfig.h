#ifndef MOTION_CONFIG_H
#define MOTION_CONFIG_H

#include <iostream>

struct MotionConfig {
  static constexpr float PLAYER_RADIUS = 18.5f;
  static constexpr float PLAYER_VELOCITY = 300.0f;
  static constexpr float PLAYER_ROTATION_VELOCITY = 360.0f;
  static constexpr std::pair<float, float> INITIAL_BALL_VELOCITY =
      std::pair<float, float>(150.0f, 150.0f);
  static constexpr float BALL_RADIUS = 12.5f;
  static constexpr float STUCK_ERROR = 2.0f;

  // SSL Ball Physics
  static constexpr float BALL_FRICTION = 150.0f;
  static constexpr float BALL_RESTITUTION = 0.8f;

  static constexpr float OBSTACLE_TOLERANCE = 30.0f;

  // Motion Constraints
  static constexpr double MOTION_MAX_SPEED_X = 300.0;
  static constexpr double MOTION_MAX_ACCEL_X = 400.0;
  static constexpr double MOTION_MAX_DECEL_X = 400.0;

  static constexpr double MOTION_MAX_SPEED_Y = 300.0;
  static constexpr double MOTION_MAX_ACCEL_Y = 400.0;
  static constexpr double MOTION_MAX_DECEL_Y = 400.0;

  static constexpr double MOTION_MAX_SPEED_THETA = 6.28;
  static constexpr double MOTION_MAX_ACCEL_THETA = 3.14;
  static constexpr double MOTION_MAX_DECEL_THETA = 3.14;
};

#endif  // MOTION_CONFIG_H