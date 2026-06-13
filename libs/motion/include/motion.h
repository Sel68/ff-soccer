#ifndef MOTION_H
#define MOTION_H

#include "SystemConstants.h"

class Motion {
 public:
  struct Point {
    double x;
    double y;
  };

  struct MotionConstraints {
    double maxSpeed;
    double maxAcceleration;
    double maxDeceleration;
  };

  struct VelocityState {
    double vx;
    double vy;
    double vtheta;
  };

  struct Profile1D {
    double maxSpeed;
    double maxAccel;
    double maxDecel;
    double distToMax;
    double distToStop;
    double cruiseTime;
    double timeToMax;
    double timeToStop;
    double totalTime;
    double v0;  // Initial velocity
  };

  struct GeneratedProfile {
    Profile1D xProfile;
    Profile1D yProfile;
    Profile1D thetaProfile;
    double totalDistanceX;
    double totalDistanceY;
    double totalDistanceTheta;
    double signX;
    double signY;
    double thetaSign;
    double totalTime;
    bool valid = false;
  };

  Motion() = default;

  // Generates the motion profile
  GeneratedProfile generateProfile(Point start, Point end, double current_theta,
                                   VelocityState initial_velocity = {0.0, 0.0, 0.0});

  // Get the velocity at time t for a generated profile
  VelocityState getVelocityState(const GeneratedProfile& profile, double t);

 private:
  struct KinematicState {
    double velocity;
    double distance;
  };

  // Plans a 1D trapezoidal or triangular motion profile
  Profile1D planProfile(double distance, MotionConstraints constraints, double v0 = 0.0);

  KinematicState calculateKinematicState(double t, const Profile1D& profile, double totalDistance);

  const MotionConstraints xDirectionConstraints = {SystemConstants::MOTION_MAX_SPEED_X,
                                                   SystemConstants::MOTION_MAX_ACCEL_X,
                                                   SystemConstants::MOTION_MAX_DECEL_X};
  const MotionConstraints yDirectionConstraints = {SystemConstants::MOTION_MAX_SPEED_Y,
                                                   SystemConstants::MOTION_MAX_ACCEL_Y,
                                                   SystemConstants::MOTION_MAX_DECEL_Y};
  const MotionConstraints thetaDirectionConstraints = {SystemConstants::MOTION_MAX_SPEED_THETA,
                                                       SystemConstants::MOTION_MAX_ACCEL_THETA,
                                                       SystemConstants::MOTION_MAX_DECEL_THETA};
};

#endif  // MOTION_H
