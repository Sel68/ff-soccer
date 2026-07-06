#ifndef OMNI_WHEELS_ANGLES_H
#define OMNI_WHEELS_ANGLES_H

/*
TODO: Need to cut down a few things in eigen to build on the microcontroller
*/

#include <Eigen/Dense>
#include <vector>

#include "SystemConstants.h"

class ChassisVelocity {
 public:
  double vx;
  double vy;
  double vtheta;
};

/* new Wheel config: x, y, phi, gamma, radius
PREV:
x = Rcos(phi)
y = Rsin(phi)
nx = cos gamma
ny = sin gamma
*/

class WheelConfig {
 public:
  double R;  // distance of wheel from the robot's center
  double phi;
  double gamma;
  double radius;  // inner wheel radius
};

class OmniKinematics {
 private:
  // TODO:
  /*
    Please use std::array with fixed number of wheels. Number of wheels
    will be a constant (constexpr). This code will go inside the microcontroller
    and the unusual growth of std::vector's heap memory is unsuitable.
  */
  std::array<WheelConfig, SystemConstants::num_wheels> wheels;
  double max_wheel_vel;

 public:
  OmniKinematics(const std::array<WheelConfig, SystemConstants::num_wheels>& wheel_configs,
                 double max_wheelspin);

  std::array<double, SystemConstants::num_wheels> ChassisToWheels(ChassisVelocity target,
                                                                  bool scale_limits = true);
  ChassisVelocity WheelsToChassis(const Eigen::VectorXd& wheel_vels);
};

#endif