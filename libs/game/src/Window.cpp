#include "Window.h"

bool keys[1024] = {};
bool keys_processed[1024] = {};

void Window::FramebufferSizeCallback(GLFWwindow *window, int width,
                                     int height) {
  glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow *gl_window, int key, int scancode, int action,
                 int mode) {}

void Window::KeyCallback(GLFWwindow *gl_window, int key, int scancode,
                         int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(gl_window, true);

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
  if (!glfwInit()) {
    std::cout << "[ERROR] [Window]: Failed to initialize GLFW" << std::endl;
    return;
  }
  std::cout << "[INFO] [Window]: GLFW Initialized" << std::endl;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, false);
  gl_window = glfwCreateWindow(SystemConstants::screen_width,
                               SystemConstants::screen_height,
                               "Soccer Simulator", nullptr, nullptr);
  if (gl_window == nullptr) {
    std::cout << "[ERROR] [Window]: Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  std::cout << "[INFO] [Window]: Window created successfully" << std::endl;
  glfwMakeContextCurrent(gl_window);

  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cout << "[ERROR] [Window]: Failed to initialize GLAD" << std::endl;
    return;
  }
  std::cout << "[INFO] [Window]: GLAD Initialized" << std::endl;
  std::cout << "[INFO] [Window]: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

  glfwSetKeyCallback(gl_window, KeyCallback);
  glfwSetFramebufferSizeCallback(gl_window, FramebufferSizeCallback);

  int fb_width, fb_height;
  glfwGetFramebufferSize(gl_window, &fb_width, &fb_height);
  glViewport(0, 0, fb_width, fb_height);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  return;
}