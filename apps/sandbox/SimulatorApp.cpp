#include <asio.hpp>
#include "Game.h"
#include "Transmitter.h"
#include "OmniKinematics.h"

int main() {
  Game soccer;
  soccer.SetCurrentAlgo(AlgoName::RRTX);

  asio::io_context ioc;
  // 192.168.1.101
  Transmitter transmitter(ioc, "127.0.0.1", 8080);
  std::vector<WheelConfig> wheel_configs = {{0.2, M_PI / 4, 0.0, 0.05},
                                            {0.2, 3 * M_PI / 4, 0.0, 0.05},
                                            {0.2, 5 * M_PI / 4, 0.0, 0.05},
                                            {0.2, 7 * M_PI / 4, 0.0, 0.05}};
  OmniKinematics kinematics(wheel_configs, 10.0);

  double dt = 0.0f;
  double last_frame = 0.0f;

  while (soccer.Running()) {
    double current_frame = glfwGetTime();
    dt = current_frame - last_frame;
    last_frame = current_frame;

    soccer.ProcessInput(dt);
    soccer.Update(dt);

    // copied transmit
    int robot_id = 0;
    for (BallObject* p : soccer.GetTeam1Players()) {
      ChassisVelocity c_vel{p->Velocity.x, p->Velocity.y, 0.0};
      Eigen::VectorXd wheels = kinematics.ChassisToWheels(c_vel);
      transmitter.transmit(robot_id++, p->Position.x, p->Position.y, 0.0, c_vel, wheels);
    }
  }

  soccer.Exit();

  return 0;
}