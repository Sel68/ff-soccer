#ifndef OMNI_WHEELS_ANGLES_H
#define OMNI_WHEELS_ANGLES_H

#include <vector>
#include <Eigen/Dense>

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
    double R;
    double phi;
    double gamma;
    double radius; // inner wheel radius
};

class OmniKinematics {
private:
    std::vector<WheelConfig> wheels;
    double max_wheel_vel;
public:
    OmniKinematics(const std::vector<WheelConfig>& wheel_configs, double max_wheelspin);

    Eigen::VectorXd chassisToWheels(ChassisVelocity target, bool scale_limits = true);
};

#endif