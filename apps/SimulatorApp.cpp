#include <asio.hpp>

#include "CommManager.h"
#include "Game.h"
#include "OmniKinematics.h"
#include "Transmitter.h"

using Clock = std::chrono::steady_clock;

int main() {
  double t_current, t_last;
  t_last = 0;

  Game soccer;
  soccer.SetCurrentAlgo(AlgoName::RRTX);

  while (soccer.Running()) {
    t_current = glfwGetTime();
    double dt = t_current - t_last;
    t_last = t_current;

    soccer.ProcessInput(dt);
    soccer.Update(dt);
  }

  soccer.Exit();

  return 0;
}