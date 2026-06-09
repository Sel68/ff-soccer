#ifndef KINEMATIC_TRANSFORMS_H
#define KINEMATIC_TRANSFORMS_H

#include "OmniKinematics.h"  //ChassisVelocity

class KinematicTransforms {
 public:
  static ChassisVelocity WorldToRobot(const ChassisVelocity& world_vel, double robot_theta);
  static ChassisVelocity RobotToWorld(const ChassisVelocity& robot_vel, double robot_theta);
};

#endif