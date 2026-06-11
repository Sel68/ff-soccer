#ifndef GAME_H
#define GAME_H

#include <tuple>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <asio.hpp>

#include <BallObject.h>
#include "Algos.h"
#include "GameLevel.h"
#include "PostProcessor.h"
#include "ResourceManager.h"
#include "Window.h"
#include "Transmitter.h"
#include "OmniKinematics.h"

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
  void ProcessInput(double dt);
  void Update(double dt);
  void Exit();

  void SetCurrentAlgo(AlgoName algo_name);
  std::vector<Point2D> Plan(std::pair<double, double> start, std::pair<double, double> goal,
                            std::vector<Obstacle> obstacles);

  bool Running();

 private:
  void Init();
  void DoCollisions();
  void Render();
  void UpdateSimulation(double dt);
  void Cleanup();

  ResourceManager resource_manager;
  SpriteRenderer* renderer;
  BallObject* ball;
  std::vector<BallObject*> team1_players;
  std::vector<BallObject*> team2_players;

  AlgoName current_algo;

  RRTX rrtx_planner;

  asio::io_context ioc;
  Transmitter* transmitter;
  OmniKinematics* kinematics;
};

#endif