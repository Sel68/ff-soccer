#ifndef MOTION_H
#define MOTION_H

#include "GameConfig.h"
#include "trajectory.h"

class Motion {
 public:
  static bool m_debug_mode;
  static void setDebugMode(bool mode) { m_debug_mode = mode; }

  struct Point {
    double x;
    double y;
  };

  struct MotionConstraints {
    double maxSpeed;
    double maxAcceleration;
    double maxDeceleration;
  };

  struct VelocityState {
    double vx;
    double vy;
    double vtheta;
  };

  struct GeneratedProfile {
    Synced2D planar;
    SingleDOF theta;
    bool valid = false;
    double totalTime = 0;
  };

  Motion() = default;

  // Generates the motion profile
  GeneratedProfile generateProfile(Point start, Point end, double current_theta,
                                   VelocityState initial_velocity = {0.0, 0.0,
                                                                     0.0});

  // Get the velocity at time t for a generated profile
  VelocityState getVelocityState(const GeneratedProfile& profile, double t);

 private:
  struct KinematicState {
    double velocity;
    double distance;
  };

  const MotionConstraints xDirectionConstraints = {
      GameConfig::motion_max_speed_x, GameConfig::motion_max_accel_x,
      GameConfig::motion_max_decel_x};
  const MotionConstraints yDirectionConstraints = {
      GameConfig::motion_max_speed_y, GameConfig::motion_max_accel_y,
      GameConfig::motion_max_decel_y};
  const MotionConstraints thetaDirectionConstraints = {
      GameConfig::motion_max_speed_theta, GameConfig::motion_max_accel_theta,
      GameConfig::motion_max_decel_theta};
};

#endif  // MOTION_H
