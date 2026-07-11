#include <OmniKinematics.h>
#include <SystemConstants.h>
#include <gtest/gtest.h>

#include <cmath>
#include <array>

const double PI = std::acos(-1.0);

class OmniKinematicsTest : public ::testing::Test {
 protected:
  std::array<WheelConfig, SystemConstants::num_wheels> wheel_configs;
  double max_spin;
  double R = 0.2;   // 20cm chassis radius
  double r = 0.05;  // 5cm wheel radius

  void SetUp() override {
    max_spin = 100.0;  // max 100 rad/s

    // Setup a standard 4-wheel omni drive (wheels at 0, 90, 180, 270 degrees)
    // Driving direction (gamma) is tangent to the circle (phi + 90 degrees)
    wheel_configs[0] = {R, 0.0, PI / 2.0, r};        // Front (on X axis)
    wheel_configs[1] = {R, PI / 2.0, PI, r};         // Left (on Y axis)
    wheel_configs[2] = {R, PI, 3.0 * PI / 2.0, r};   // Back (on -X axis)
    wheel_configs[3] = {R, 3.0 * PI / 2.0, 0.0, r};  // Right (on -Y axis)
  }
};

TEST_F(OmniKinematicsTest, ZeroVelocity) {
  OmniKinematics kin(wheel_configs, max_spin);
  ChassisVelocity target = {0.0, 0.0, 0.0};

  std::array<double, SystemConstants::num_wheels> vels = kin.ChassisToWheels(target);

  ASSERT_EQ(vels.size(), 4);
  for (int i = 0; i < 4; ++i) {
    EXPECT_NEAR(vels[i], 0.0, 1e-6);
  }
}

TEST_F(OmniKinematicsTest, ForwardVelocityX) {
  OmniKinematics kin(wheel_configs, max_spin);
  ChassisVelocity target = {1.0, 0.0, 0.0};  // Move +1.0 m/s in X

  std::array<double, SystemConstants::num_wheels> vels = kin.ChassisToWheels(target);

  // W1 (gamma=pi/2): cos(pi/2)*1 = 0
  // W2 (gamma=pi): cos(pi)*1 = -1 -> vel = -1/0.05 = -20 rad/s
  // W3 (gamma=3pi/2): cos(3pi/2)*1 = 0
  // W4 (gamma=0): cos(0)*1 = 1 -> vel = 1/0.05 = 20 rad/s

  EXPECT_NEAR(vels[0], 0.0, 1e-6);
  EXPECT_NEAR(vels[1], -20.0, 1e-6);
  EXPECT_NEAR(vels[2], 0.0, 1e-6);
  EXPECT_NEAR(vels[3], 20.0, 1e-6);
}

TEST_F(OmniKinematicsTest, ForwardVelocityY) {
  OmniKinematics kin(wheel_configs, max_spin);
  ChassisVelocity target = {0.0, 1.0, 0.0};  // Move +1.0 m/s in Y

  std::array<double, SystemConstants::num_wheels> vels = kin.ChassisToWheels(target);

  // W1 (gamma=pi/2): sin(pi/2)*1 = 1 -> 20 rad/s
  // W2 (gamma=pi): sin(pi)*1 = 0
  // W3 (gamma=3pi/2): sin(3pi/2)*1 = -1 -> -20 rad/s
  // W4 (gamma=0): sin(0)*1 = 0

  EXPECT_NEAR(vels[0], 20.0, 1e-6);
  EXPECT_NEAR(vels[1], 0.0, 1e-6);
  EXPECT_NEAR(vels[2], -20.0, 1e-6);
  EXPECT_NEAR(vels[3], 0.0, 1e-6);
}

TEST_F(OmniKinematicsTest, RotationPure) {
  OmniKinematics kin(wheel_configs, max_spin);
  ChassisVelocity target = {0.0, 0.0, 1.0};  // Rotate +1.0 rad/s

  std::array<double, SystemConstants::num_wheels> vels = kin.ChassisToWheels(target);

  // For each wheel: v_w = R * sin(gamma - phi) * vtheta
  // gamma - phi = pi/2 for all wheels. sin(pi/2) = 1
  // v_w = 0.2 * 1 * 1.0 = 0.2 m/s
  // wheel_vel = 0.2 / 0.05 = 4.0 rad/s

  for (int i = 0; i < 4; ++i) {
    EXPECT_NEAR(vels[i], 4.0, 1e-6);
  }
}

// TEST_F(OmniKinematicsTest, ScaleLimits) {
//   OmniKinematics kin(wheel_configs, 10.0);  // max spin is 10 rad/s
//   ChassisVelocity target = {1.0, 0.0,
//                             0.0};  // Move +1.0 m/s in X (would normally require 20 rad/s)

//   std::array<double, SystemConstants::num_wheels> vels =
//       kin.ChassisToWheels(target, true);  // scale limits = true

//   // W2 would be -20, W4 would be +20. Max observed is 20. Limit is 10.
//   // Lambda = 10 / 20 = 0.5.
//   // Expected: W2 = -10, W4 = 10.

//   EXPECT_NEAR(vels[0], 0.0, 1e-6);
//   EXPECT_NEAR(vels[1], -10.0, 1e-6);
//   EXPECT_NEAR(vels[2], 0.0, 1e-6);
//   EXPECT_NEAR(vels[3], 10.0, 1e-6);
// }

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
