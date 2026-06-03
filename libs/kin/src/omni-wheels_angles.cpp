#include "omni-wheels_angles.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <Eigen/Dense>

OmniKinematics::OmniKinematics(const std::vector<WheelConfig>& wheel_configs, double max_wheelspin) :
    wheels(wheel_configs), max_wheel_vel(max_wheelspin) {}

Eigen::VectorXd OmniKinematics::chassisToWheels(ChassisVelocity target, bool scale_limits) {
    
    Eigen::VectorXd wheel_vels(wheels.size());
    double max_observed = 0.0;

    for (size_t i = 0; i < wheels.size(); ++i) {

        double v_w_i = std::cos(wheels[i].gamma) * target.vx +
                       std::sin(wheels[i].gamma) * target.vy +
                       wheels[i].R * std::sin(wheels[i].gamma - wheels[i].phi) * target.vtheta;

        wheel_vels[i] = v_w_i / wheels[i].radius;
        max_observed = std::max(max_observed, std::abs(wheel_vels[i]));
    }

    if (scale_limits && max_observed > max_wheel_vel) {
        double lambda = max_wheel_vel / max_observed;
        wheel_vels *= lambda;
    }
    return wheel_vels;
}
