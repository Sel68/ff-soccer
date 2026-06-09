// IMPORTANT: glad should come first. Then GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Game.h"
#include "ResourceManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const unsigned int SCREEN_WIDTH = 950;
const unsigned int SCREEN_HEIGHT = 580;

class window {
 public:
  void windowinit();
};