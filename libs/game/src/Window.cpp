#include "Window.h"

bool keys[1024] = {};
bool keys_processed[1024] = {};

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* gl_window, int key, int scancode, int action, int mode) {}

void Window::KeyCallback(GLFWwindow* gl_window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(gl_window, true);

  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      keys[key] = true;
    else if (action == GLFW_RELEASE) {
      keys[key] = false;
      keys_processed[key] = false;
    }
  }
}

void Window::WindowInit() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, false);
  gl_window = glfwCreateWindow(SystemConstants::SCREEN_WIDTH, SystemConstants::SCREEN_HEIGHT,
                               "Breakout", nullptr, nullptr);
  glfwMakeContextCurrent(gl_window);

  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  glfwSetKeyCallback(gl_window, KeyCallback);
  glfwSetFramebufferSizeCallback(gl_window, FramebufferSizeCallback);

  glViewport(0, 0, SystemConstants::SCREEN_WIDTH, SystemConstants::SCREEN_HEIGHT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  return;
}