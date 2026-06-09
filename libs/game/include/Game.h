#ifndef GAME_H
#define GAME_H
#include <BallObject.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>
#include <vector>

#include "Constants.h"
#include "GameLevel.h"
#include "PostProcessor.h"
#include "ResourceManager.h"
#include "Window.h"

enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

enum Direction { UP, RIGHT, DOWN, LEFT };

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
 public:
  GameState State;
  bool Keys[1024];
  bool KeysProcessed[1024];
  unsigned int Width, Height;
  Window game_window;
  std::vector<GameLevel> Levels;
  unsigned int Level;
  unsigned int Lives;

  Game();
  ~Game();
  void Init();
  void Cleanup();
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();
  void DoCollisions();

 private:
  ResourceManager resource_manager;

  SpriteRenderer* renderer;
  BallObject* ball;
  std::vector<BallObject*> team1_players;
  std::vector<BallObject*> team2_players;
};

#endif