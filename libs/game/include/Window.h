#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

// IMPORTANT: glad should come first. Then GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Constants.h"
#include "Game.h"
#include "ResourceManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

class Window {
 public:
  void windowinit();
};

#endif  // GAME_WINDOW_H