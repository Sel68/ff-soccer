#include <cmath>
#include <algorithm>
#include <iostream>

#include "../include/motion.h"

bool Motion::m_debug_mode = false;

Motion::Profile1D Motion::planProfile(double distance, MotionConstraints constraints, double v0) {
  Profile1D p;
  p.v0 = std::min(std::abs(v0), constraints.maxSpeed);
  p.maxSpeed = constraints.maxSpeed;
  p.maxAccel = constraints.maxAcceleration;
  p.maxDecel = constraints.maxDeceleration;

  p.distToMax = (p.maxSpeed * p.maxSpeed - p.v0 * p.v0) / (2.0 * p.maxAccel);
  p.distToStop = (p.maxSpeed * p.maxSpeed) / (2.0 * p.maxDecel);

  if (distance > p.distToMax + p.distToStop) {
    // Trapezoidal motion calculation
    p.cruiseTime = (distance - p.distToMax - p.distToStop) / p.maxSpeed;
    p.timeToMax = (p.maxSpeed - p.v0) / p.maxAccel;
    p.timeToStop = p.maxSpeed / p.maxDecel;
    if (m_debug_mode) {
      std::cout << "[DEBUG] [Motion]: Trapezoidal Profile -> Accel: " << p.timeToMax 
                << "s | Cruise: " << p.cruiseTime << "s | Decel: " << p.timeToStop << "s" << std::endl;
    }
  } else {
    // Triangular motion calculation
    p.maxSpeed = std::sqrt((distance + (p.v0 * p.v0) / (2.0 * p.maxAccel)) /
                           (1.0 / (2.0 * p.maxAccel) + 1.0 / (2.0 * p.maxDecel)));
    if (p.maxSpeed < p.v0) {
      p.maxSpeed = p.v0;
      p.timeToMax = 0.0;
      p.distToMax = 0.0;
    } else {
      p.timeToMax = (p.maxSpeed - p.v0) / p.maxAccel;
      p.distToMax = (p.maxSpeed * p.maxSpeed - p.v0 * p.v0) / (2.0 * p.maxAccel);
    }
    p.timeToStop = p.maxSpeed / p.maxDecel;
    p.distToStop = (p.maxSpeed * p.maxSpeed) / (2.0 * p.maxDecel);
    p.cruiseTime = 0.0;
    if (m_debug_mode) {
      std::cout << "[DEBUG] [Motion]: Triangular Profile -> Accel: " << p.timeToMax 
                << "s | Cruise: " << p.cruiseTime << "s | Decel: " << p.timeToStop << "s" << std::endl;
    }
  }

  // Total time calculation
  p.totalTime = p.timeToMax + p.cruiseTime + p.timeToStop;
  return p;
}

Motion::GeneratedProfile Motion::generateProfile(Point start, Point end, double current_theta,
                                                 VelocityState initial_velocity) {
  GeneratedProfile profile;
  profile.valid = true;

  // Calculate displacement and direction
  double displacementX = end.x - start.x;
  double displacementY = end.y - start.y;

  double totalDistanceX = std::abs(displacementX);
  double totalDistanceY = std::abs(displacementY);

  profile.signX = (displacementX >= 0) ? 1.0 : -1.0;
  profile.signY = (displacementY >= 0) ? 1.0 : -1.0;
  profile.totalDistanceX = totalDistanceX;
  profile.totalDistanceY = totalDistanceY;

  // TODO: Why?
  double targetThetaRaw = std::atan2(displacementY, displacementX);
  double displacementTheta = targetThetaRaw - current_theta;

  // Normalize angle to [-PI, PI]
  while (displacementTheta > M_PI) displacementTheta -= 2.0 * M_PI;
  while (displacementTheta <= -M_PI) displacementTheta += 2.0 * M_PI;

  profile.thetaSign = (displacementTheta >= 0) ? 1.0 : -1.0;
  double totalDistanceTheta = std::abs(displacementTheta);
  profile.totalDistanceTheta = totalDistanceTheta;

  // Initial calculations using the planProfile helper
  profile.xProfile = planProfile(totalDistanceX, xDirectionConstraints, initial_velocity.vx);
  profile.yProfile = planProfile(totalDistanceY, yDirectionConstraints, initial_velocity.vy);
  profile.thetaProfile =
      planProfile(totalDistanceTheta, thetaDirectionConstraints, initial_velocity.vtheta);

  // Time-synchronization scaling
  if (profile.xProfile.totalTime > 0.0 && profile.yProfile.totalTime > 0.0) {
    if (profile.xProfile.totalTime < profile.yProfile.totalTime) {
      if (m_debug_mode) std::cout << "[DEBUG] [Motion]: Time-syncing axes. Throttling X-axis constraints." << std::endl;
      double ratio = profile.xProfile.totalTime / profile.yProfile.totalTime;
      MotionConstraints scaledConstraints = {
          xDirectionConstraints.maxSpeed * ratio,
          xDirectionConstraints.maxAcceleration * ratio * ratio,
          xDirectionConstraints.maxDeceleration * ratio * ratio};
      profile.xProfile = planProfile(totalDistanceX, scaledConstraints, initial_velocity.vx);
    } else if (profile.yProfile.totalTime < profile.xProfile.totalTime) {
      if (m_debug_mode) std::cout << "[DEBUG] [Motion]: Time-syncing axes. Throttling Y-axis constraints." << std::endl;
      double ratio = profile.yProfile.totalTime / profile.xProfile.totalTime;
      MotionConstraints scaledConstraints = {
          yDirectionConstraints.maxSpeed * ratio,
          yDirectionConstraints.maxAcceleration * ratio * ratio,
          yDirectionConstraints.maxDeceleration * ratio * ratio};
      profile.yProfile = planProfile(totalDistanceY, scaledConstraints, initial_velocity.vy);
    }
  }

  profile.totalTime = std::max(
      {profile.xProfile.totalTime, profile.yProfile.totalTime, profile.thetaProfile.totalTime});

  if (m_debug_mode) {
    std::cout << "[DEBUG] [Motion]: Generated new trajectory. Total Time: " << profile.totalTime << "s" << std::endl;
  }

  return profile;
}

Motion::VelocityState Motion::getVelocityState(const GeneratedProfile& profile, double t) {
  VelocityState v{0.0, 0.0, 0.0};
  if (!profile.valid) return v;

  KinematicState xState = calculateKinematicState(t, profile.xProfile, profile.totalDistanceX);
  KinematicState yState = calculateKinematicState(t, profile.yProfile, profile.totalDistanceY);
  KinematicState thetaState =
      calculateKinematicState(t, profile.thetaProfile, profile.totalDistanceTheta);

  v.vx = xState.velocity * profile.signX;
  v.vy = yState.velocity * profile.signY;
  v.vtheta = thetaState.velocity * profile.thetaSign;

  return v;
}

Motion::KinematicState Motion::calculateKinematicState(double t, const Profile1D& profile,
                                                       double totalDistance) {
  KinematicState state{0.0, 0.0};

  if (t < profile.timeToMax) {
    state.velocity = profile.v0 + profile.maxAccel * t;
    state.distance = profile.v0 * t + 0.5 * profile.maxAccel * t * t;
  } else if (t < profile.timeToMax + profile.cruiseTime) {
    double dt = t - profile.timeToMax;
    state.velocity = profile.maxSpeed;
    state.distance = profile.distToMax + profile.maxSpeed * dt;
  } else {
    double t_rem = profile.totalTime - t;
    if (t_rem < 0) t_rem = 0;
    state.velocity = profile.maxDecel * t_rem;
    state.distance = totalDistance - 0.5 * profile.maxDecel * t_rem * t_rem;
  }

  return state;
}