#include <KinematicTransforms.h>
#include <gtest/gtest.h>

#include <cmath>

const double PI = std::acos(-1.0);

TEST(KinematicTransformsTest, WorldToRobot) {
  ChassisVelocity world_vel = {1.0, 0.0, 0.5};  // moving +x in world
  double theta = PI / 2.0;                      // robot is facing +y

  ChassisVelocity robot_vel = KinematicTransforms::WorldToRobot(world_vel, theta);

  // In robot frame, world +x is robot -y.
  EXPECT_NEAR(robot_vel.vx, 0.0, 1e-6);
  EXPECT_NEAR(robot_vel.vy, -1.0, 1e-6);
  EXPECT_NEAR(robot_vel.vtheta, 0.5, 1e-6);
}

TEST(KinematicTransformsTest, RobotToWorld) {
  ChassisVelocity robot_vel = {1.0, 0.0, -0.5};  // moving forward in robot frame
  double theta = PI / 2.0;                       // robot is facing +y

  ChassisVelocity world_vel = KinematicTransforms::RobotToWorld(robot_vel, theta);

  // In world frame, robot +x is world +y.
  EXPECT_NEAR(world_vel.vx, 0.0, 1e-6);
  EXPECT_NEAR(world_vel.vy, 1.0, 1e-6);
  EXPECT_NEAR(world_vel.vtheta, -0.5, 1e-6);
}
