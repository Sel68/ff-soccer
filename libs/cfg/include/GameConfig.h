#ifndef MOTION_CONFIG_H
#define MOTION_CONFIG_H

#include <iostream>

/* TODO:  GameConfig
          Logs while starting things up in Game (Resource Load...) Error
   handling Namespaces: every lib, namespace itself same
*/
struct GameConfig {
  static constexpr float player_radius = 18.5f;
  static constexpr float player_velocity = 20.0f;
  static constexpr float player_rotation_velocity = 360.0f;
  static constexpr std::pair<float, float> initial_ball_velocity =
      std::pair<float, float>(150.0f, 150.0f);
  static constexpr float ball_radius = 12.5f;
  static constexpr float stuck_error = 2.0f;

  // SSL Ball Physics
  static constexpr float ball_friction = 150.0f;
  static constexpr float ball_restitution = 0.8f;

  static constexpr float obstacle_tolerance = 30.0f;

  // Motion Constraints
  static constexpr double motion_max_speed_x = 300.0;
  static constexpr double motion_max_accel_x = 400.0;
  static constexpr double motion_max_decel_x = 400.0;

  static constexpr double motion_max_speed_y = 300.0;
  static constexpr double motion_max_accel_y = 400.0;
  static constexpr double motion_max_decel_y = 400.0;

  static constexpr double motion_max_speed_theta = 6.28;
  static constexpr double motion_max_accel_theta = 3.14;
  static constexpr double motion_max_decel_theta = 3.14;
};

#endif // MOTION_CONFIG_H