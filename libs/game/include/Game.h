#ifndef GAME_H
#define GAME_H

#include <asio.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Algos.h"
#include "GameObject.h"
#include "MainStrategy.h"
#include "PostProcessor.h"
#include "ResourceManager.h"
#include "Window.h"

enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

enum Direction { UP, RIGHT, DOWN, LEFT };

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
 public:
  Game();
  ~Game();
  void ProcessInput(double dt, double posX, double posY, double theta);
  void Update(double dt);
  void Exit();

  void SetCurrentAlgo(AlgoName algo_name);
  std::vector<Point2D> Plan(std::pair<double, double> start, std::pair<double, double> goal,
                            std::vector<Obstacle> obstacles);

  bool Running();

  const std::vector<GameObject*>& GetTeam1Players() const;

 private:
  void Init();
  void DoCollisions();
  void Render();
  void ObjectPosInit();
  void UpdateSimulation(double dt);
  void Cleanup();

  void HandleManualKick();
  void UpdateAutoStrategy(GameObject* movableBot, double dt);
  void UpdateManualMotion(GameObject* robot, double dt);
  void ProcessDebugKeys();
  void ProcessPlayerInput(GameObject* player, double dt, double posX, double posY, double theta);
  void ProcessMouseClickMovement();
  void HandleBallCollision(GameObject* player);
  void HandleBotCollision(GameObject* p1, GameObject* p2);

  GameObject* ball;
  std::vector<GameObject*> team1_players;
  std::vector<GameObject*> team2_players;
  GameState state;

  // Graphics
  Window game_window;
  unsigned int level;
  ResourceManager resource_manager;
  SpriteRenderer* renderer;

  // Algo
  AlgoName current_algo;
  RRTX rrtx_planner;
  MainStrategy m_strategy;

  bool is_auto_mode = true;
  bool passive = false;
};

#endif