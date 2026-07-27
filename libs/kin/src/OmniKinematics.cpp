#include "OmniKinematics.h"

#include <algorithm>
#include <cmath>

OmniKinematics::OmniKinematics(
    const std::array<WheelConfig, SystemConstants::num_drive_motors>& wheel_configs,
    double max_wheelspin)
    : wheels(wheel_configs), max_wheel_vel(max_wheelspin) {}

std::array<double, SystemConstants::num_drive_motors> OmniKinematics::ChassisToWheels(
    ChassisVelocity target) {
  std::array<double, SystemConstants::num_drive_motors> wheel_vels;
  double max_observed = 0.0;

  for (size_t i = 0; i < wheels.size(); ++i) {
    double v_w_i = std::cos(wheels[i].gamma) * target.vx + std::sin(wheels[i].gamma) * target.vy +
                   wheels[i].R * std::sin(wheels[i].gamma - wheels[i].phi) * target.vtheta;

    wheel_vels[i] = v_w_i / wheels[i].radius;
    max_observed = std::max(max_observed, std::abs(wheel_vels[i]));
  }

  // if (scale_limits && max_observed > max_wheel_vel) {
  //   double lambda = max_wheel_vel / max_observed;
  //   wheel_vels[i] *= lambda;
  // }
  return wheel_vels;
}

ChassisVelocity OmniKinematics::WheelsToChassis(
    const std::array<double, SystemConstants::num_drive_motors>& wheel_vels) {
  double M[SystemConstants::num_drive_motors][3];
  double V_w[SystemConstants::num_drive_motors];

  for (size_t i = 0; i < SystemConstants::num_drive_motors; ++i) {
    M[i][0] = std::cos(wheels[i].gamma);
    M[i][1] = std::sin(wheels[i].gamma);
    M[i][2] = wheels[i].R * std::sin(wheels[i].gamma - wheels[i].phi);

    // Convert angular velocity of wheel to linear velocity of the wheel's contact point
    V_w[i] = wheel_vels[i] * wheels[i].radius;
  }

  // M_T_M = M^T * M (3x3)
  double M_T_M[3][3] = {0};
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      for (size_t k = 0; k < SystemConstants::num_drive_motors; ++k) {
        M_T_M[i][j] += M[k][i] * M[k][j];
      }
    }
  }

  // Inverse of 3x3 matrix M_T_M
  double det = M_T_M[0][0] * (M_T_M[1][1] * M_T_M[2][2] - M_T_M[2][1] * M_T_M[1][2]) -
               M_T_M[0][1] * (M_T_M[1][0] * M_T_M[2][2] - M_T_M[1][2] * M_T_M[2][0]) +
               M_T_M[0][2] * (M_T_M[1][0] * M_T_M[2][1] - M_T_M[1][1] * M_T_M[2][0]);

  if (std::abs(det) < 1e-6) {
    return {0.0, 0.0, 0.0};
  }

  double invDet = 1.0 / det;
  double M_T_M_inv[3][3];
  M_T_M_inv[0][0] = (M_T_M[1][1] * M_T_M[2][2] - M_T_M[2][1] * M_T_M[1][2]) * invDet;
  M_T_M_inv[0][1] = (M_T_M[0][2] * M_T_M[2][1] - M_T_M[0][1] * M_T_M[2][2]) * invDet;
  M_T_M_inv[0][2] = (M_T_M[0][1] * M_T_M[1][2] - M_T_M[0][2] * M_T_M[1][1]) * invDet;
  M_T_M_inv[1][0] = (M_T_M[1][2] * M_T_M[2][0] - M_T_M[1][0] * M_T_M[2][2]) * invDet;
  M_T_M_inv[1][1] = (M_T_M[0][0] * M_T_M[2][2] - M_T_M[0][2] * M_T_M[2][0]) * invDet;
  M_T_M_inv[1][2] = (M_T_M[1][0] * M_T_M[0][2] - M_T_M[0][0] * M_T_M[1][2]) * invDet;
  M_T_M_inv[2][0] = (M_T_M[1][0] * M_T_M[2][1] - M_T_M[2][0] * M_T_M[1][1]) * invDet;
  M_T_M_inv[2][1] = (M_T_M[2][0] * M_T_M[0][1] - M_T_M[0][0] * M_T_M[2][1]) * invDet;
  M_T_M_inv[2][2] = (M_T_M[0][0] * M_T_M[1][1] - M_T_M[1][0] * M_T_M[0][1]) * invDet;

  // pseudo_inv = M_T_M_inv * M^T (3xnum_drive_motors)
  double pseudo_inv[3][SystemConstants::num_drive_motors] = {0};
  for (int i = 0; i < 3; ++i) {
    for (size_t j = 0; j < SystemConstants::num_drive_motors; ++j) {
      for (int k = 0; k < 3; ++k) {
        pseudo_inv[i][j] += M_T_M_inv[i][k] * M[j][k];  // M[j][k] is M^T[k][j]
      }
    }
  }

  // V_c = pseudo_inv * V_w (3x1)
  double V_c[3] = {0};
  for (int i = 0; i < 3; ++i) {
    for (size_t j = 0; j < SystemConstants::num_drive_motors; ++j) {
      V_c[i] += pseudo_inv[i][j] * V_w[j];
    }
  }

  return {V_c[0], V_c[1], V_c[2]};
}
