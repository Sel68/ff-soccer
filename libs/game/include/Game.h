#ifndef GAME_H
#define GAME_H
#include <BallObject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>
#include <vector>

#include "Algos.h"
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
  unsigned int Width, Height;
  Window game_window;
  std::vector<GameLevel> Levels;
  unsigned int Level;
  unsigned int Lives;

  Game();
  ~Game();
  void ProcessInput(float dt);
  void Update(float dt);
  void Exit();

  void SetCurrentAlgo(AlgoName algo_name);
  std::vector<Point2D> Plan(std::pair<double, double> start, std::pair<double, double> goal,
                            std::vector<Obstacle> obstacles);

  bool Running();

 private:
  void Init();
  void DoCollisions();
  void Render();
  void Cleanup();

  ResourceManager resource_manager;
  SpriteRenderer* renderer;
  BallObject* ball;
  std::vector<BallObject*> team1_players;
  std::vector<BallObject*> team2_players;

  AlgoName current_algo;

  RRTX rrtx_planner;
};

#endif