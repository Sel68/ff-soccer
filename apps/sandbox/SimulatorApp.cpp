#include "Game.h"

int main() {
  Game soccer;
  soccer.SetCurrentAlgo(AlgoName::RRTX);

  double dt = 0.0f;
  double last_frame = 0.0f;

  while (soccer.Running()) {
    double current_frame = glfwGetTime();
    dt = current_frame - last_frame;
    last_frame = current_frame;

    soccer.ProcessInput(dt);
    soccer.Update(dt);
  }

  soccer.Exit();

  return 0;
}