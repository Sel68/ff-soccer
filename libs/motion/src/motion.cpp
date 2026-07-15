#include "../include/motion.h"

#include <algorithm>
#include <cmath>
#include <iostream>

/*
  MotionManager: (0, 0, 0) -> (1, 1, 1) -> ...
    current_trajectory

  Simple Path: Two waypoints (start, goal) -> GenerateMotion(start, goal)

  Trajectory's mathematical implementation.
    Start with a simple curve
    Then trapezoidal, triangular
    FINAL: S Curves

  GetVelocity(t):
    current_trajectory(...)

      /-----------\
     /             \
    /               \

  speed will be given to the game robot

  In Simulator App:
    Start, Goal => Robot moves from start to goal
*/

bool Motion::m_debug_mode = false;

Motion::GeneratedProfile Motion::generateProfile(
    Point start, Point end, double current_theta,
    VelocityState initial_velocity) {

    // Debug
std::cout << "generateProfile this = " << this << '\n';

  // TODO: We don't need different x and y max accel and speed because the
  // algorithm only needs one. because, we can't make x faster than y anyway in
  // our physical robot.

  // Note that Synced2D is our motion profiling algorithm here. 
  // It combines two SingleDOF.
    Synced2D planar({start.x, start.y}, {end.x, end.y}, {initial_velocity.vx, initial_velocity.vy},
               GameConfig::motion_max_speed_x, GameConfig::motion_max_accel_x);

  // TODO: merge theta as part of the Synced DOF.
  double targetTheta = 45;
  SingleDOF thetaDOF(current_theta, targetTheta, initial_velocity.vtheta,
                      GameConfig::motion_max_speed_theta,
                      GameConfig::motion_max_accel_theta);

  return GeneratedProfile {
    .planar = planar,
    .theta = thetaDOF,
    .valid = true,
    .totalTime = planar.totalTime()
  };
}

Motion::VelocityState Motion::getVelocityState(const GeneratedProfile& p,
                                               double t) {
  VelocityState v{0.0, 0.0, 0.0};
  if (!p.valid) return v;

  v.vx = p.planar.velocityAtX(t);
  v.vy = p.planar.velocityAtY(t);

  v.vtheta = p.theta.velocityAt(t);

  return v;
}
