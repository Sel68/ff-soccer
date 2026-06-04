#include <cmath>

#include "KinematicTransforms.h"

ChassisVelocity KinematicTransforms::worldToRobot(const ChassisVelocity& world_vel,
                                                  double robot_theta) {
  ChassisVelocity robot_vel;
  robot_vel.vx = world_vel.vx * std::cos(robot_theta) + world_vel.vy * std::sin(robot_theta);
  robot_vel.vy = -world_vel.vx * std::sin(robot_theta) + world_vel.vy * std::cos(robot_theta);
  robot_vel.vtheta = world_vel.vtheta;
  return robot_vel;
}

ChassisVelocity KinematicTransforms::robotToWorld(const ChassisVelocity& robot_vel,
                                                  double robot_theta) {
  ChassisVelocity world_vel;
  world_vel.vx = robot_vel.vx * std::cos(robot_theta) - robot_vel.vy * std::sin(robot_theta);
  world_vel.vy = robot_vel.vx * std::sin(robot_theta) + robot_vel.vy * std::cos(robot_theta);
  world_vel.vtheta = robot_vel.vtheta;
  return world_vel;
}
