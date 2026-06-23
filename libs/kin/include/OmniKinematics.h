#ifndef OMNI_WHEELS_ANGLES_H
#define OMNI_WHEELS_ANGLES_H

#include <array>
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
  std::array<WheelConfig, SystemConstants::num_wheels> wheels;
  double max_wheel_vel;

 public:
  OmniKinematics(const std::array<WheelConfig, SystemConstants::num_wheels>& wheel_configs,
                 double max_wheelspin);

  std::array<double, SystemConstants::num_wheels> ChassisToWheels(ChassisVelocity target,
                                                                  bool scale_limits = true);
  ChassisVelocity WheelsToChassis(
      const std::array<double, SystemConstants::num_wheels>& wheel_vels);
};

#endif