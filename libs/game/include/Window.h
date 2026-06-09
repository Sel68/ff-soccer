#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <iostream>

#include "ResourceManager.h"
#include "SystemConstants.h"

extern bool keys[1024];
extern bool keys_processed[1024];

class Window {
 public:
  void WindowInit();
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

  GLFWwindow* gl_window;

 private:
};

#endif  // GAME_WINDOW_H