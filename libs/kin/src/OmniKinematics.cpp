#include <cmath>
#include <vector>
#include <algorithm>
#include <Eigen/Dense>

#include "OmniKinematics.h"

OmniKinematics::OmniKinematics(const std::vector<WheelConfig>& wheel_configs, double max_wheelspin)
    : wheels(wheel_configs), max_wheel_vel(max_wheelspin) {}

Eigen::VectorXd OmniKinematics::chassisToWheels(ChassisVelocity target, bool scale_limits) {
  Eigen::VectorXd wheel_vels(wheels.size());
  double max_observed = 0.0;

  for (size_t i = 0; i < wheels.size(); ++i) {
    double v_w_i = std::cos(wheels[i].gamma) * target.vx + std::sin(wheels[i].gamma) * target.vy +
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

ChassisVelocity OmniKinematics::wheelsToChassis(const Eigen::VectorXd& wheel_vels) {
  int n = wheels.size();
  if (n == 0) return {0.0, 0.0, 0.0};

  Eigen::MatrixXd M(n, 3);
  Eigen::VectorXd V_w(n);

  for (int i = 0; i < n; ++i) {
    M(i, 0) = std::cos(wheels[i].gamma);
    M(i, 1) = std::sin(wheels[i].gamma);
    M(i, 2) = wheels[i].R * std::sin(wheels[i].gamma - wheels[i].phi);

    // Convert angular velocity of wheel to linear velocity of the wheel's contact point
    V_w(i) = wheel_vels[i] * wheels[i].radius;
  }

  // solve M * V_c = V_w using least squares via QR decomposition
  Eigen::Vector3d V_c = M.colPivHouseholderQr().solve(V_w);

  return {V_c(0), V_c(1), V_c(2)};
}
