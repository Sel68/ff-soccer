#ifndef SYSTEM_CONSTANTS
#define SYSTEM_CONSTANTS

#include <utility>
#include <cstdint>
#include <array>

struct SystemConstants {
  static constexpr float PLAYER_RADIUS = 18.5f;
  static constexpr float PLAYER_VELOCITY = 300.0f;
  static constexpr float PLAYER_ROTATION_VELOCITY = 360.0f;
  static constexpr std::pair<float, float> INITIAL_BALL_VELOCITY =
      std::pair<float, float>(150.0f, 150.0f);
  static constexpr float BALL_RADIUS = 12.5f;
  static constexpr float Stuckerror = 2.0f;

  // SSL Ball Physics
  static constexpr float BALL_FRICTION = 150.0f;   // Linear deceleration (px/s^2)
  static constexpr float BALL_RESTITUTION = 0.8f;  // 20% energy loss on bounce

  static constexpr unsigned int SCREEN_WIDTH = 950;
  static constexpr unsigned int SCREEN_HEIGHT = 580;

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

struct NetworkConfig {
  // IPs (192.168.1.XYZ)
  static constexpr uint8_t base_station_ip = 100;
  static constexpr uint8_t host_ip = 101;

  // Ports
  static constexpr uint16_t host_listen_port = 8080;

  // Message sizes
  static constexpr int udp_buffer_max_size = 1024;
};

#endif  // SYSTEM_CONSTANTS
