#include "Game.h"

int main() {
  Game soccer;

  float dt = 0.0f;
  float last_frame = 0.0f;

  while (soccer.Running()) {
    float current_frame = glfwGetTime();
    dt = current_frame - last_frame;
    last_frame = current_frame;

    soccer.ProcessInput(dt);
    soccer.Update(dt);
  }

  soccer.Exit();



  return 0;
}