#include "Game.h"
#include "RRTX.h"

#ifndef SHADER_DIR
#define SHADER_DIR ""
#endif

#ifndef TEXTURE_DIR
#define TEXTURE_DIR ""
#endif

#ifndef LEVEL_DIR
#define LEVEL_DIR ""
#endif

Game::Game() : State(GAME_ACTIVE) {
  this->Width = GameConstants::SCREEN_WIDTH;
  this->Height = GameConstants::SCREEN_HEIGHT;
  game_window.WindowInit();
  Init();
}

void Game::Exit() {
  Cleanup();
  resource_manager.Clear();

  glfwTerminate();
}

Game::~Game() { Exit(); }

bool Game::Running() { return !glfwWindowShouldClose(game_window.gl_window); }

void Game::Cleanup() {
  delete renderer;
  renderer = nullptr;

  for (BallObject* p : team1_players) delete p;
  team1_players.clear();

  for (BallObject* p : team2_players) delete p;
  team2_players.clear();

  delete ball;
  ball = nullptr;
}

void Game::Init() {
  resource_manager.LoadShader(SHADER_DIR "sprite.vs", SHADER_DIR "sprite.fs", nullptr, "sprite");
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

  resource_manager.GetShader("sprite").Use().SetInteger("image", 0);
  resource_manager.GetShader("sprite").SetMatrix4("projection", projection);
  resource_manager.LoadTexture(TEXTURE_DIR "background.jpg", false, "background");
  resource_manager.LoadTexture(TEXTURE_DIR "awesomeface.png", true, "face");
  resource_manager.LoadTexture(TEXTURE_DIR "robot.png", true, "robot");

  static Shader shader = resource_manager.GetShader("sprite");
  renderer = new SpriteRenderer(shader);

  GameLevel one;
  one.Load(resource_manager, LEVEL_DIR "sim.lvl", this->Width, this->Height / 2);

  this->Levels.push_back(one);
  this->Level = 0;

  // Team 1 players
  glm::vec2 playerPos1 = glm::vec2(this->Width * 0.125f,
                                   this->Height * 0.5f - GameConstants::PLAYER_RADIUS);  // keeper
  BallObject* p1 =
      new BallObject(playerPos1, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), false);
  team1_players.push_back(p1);

  glm::vec2 playerPos2 = glm::vec2(
      this->Width * 0.25f, this->Height * 0.25f - GameConstants::PLAYER_RADIUS);  // defender1
  BallObject* p2 =
      new BallObject(playerPos2, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p2);

  glm::vec2 playerPos3 = glm::vec2(
      this->Width * 0.25f, this->Height * 0.75f - GameConstants::PLAYER_RADIUS);  // defender2
  BallObject* p3 =
      new BallObject(playerPos3, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p3);

  glm::vec2 playerPos4 = glm::vec2(
      this->Width * 0.4f, this->Height * 0.15f - GameConstants::PLAYER_RADIUS);  // midfielder1
  BallObject* p4 =
      new BallObject(playerPos4, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p4);

  glm::vec2 playerPos5 = glm::vec2(
      this->Width * 0.4f, this->Height * 0.5f - GameConstants::PLAYER_RADIUS);  // midfielder2
  BallObject* p5 =
      new BallObject(playerPos5, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p5);

  glm::vec2 playerPos6 = glm::vec2(
      this->Width * 0.4f, this->Height * 0.85f - GameConstants::PLAYER_RADIUS);  // midfielder3
  BallObject* p6 =
      new BallObject(playerPos6, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p6);

  // Team 2 players
  glm::vec2 TplayerPos1 = glm::vec2(this->Width * 0.875f,
                                    this->Height * 0.5f - GameConstants::PLAYER_RADIUS);  // keeper
  BallObject* Tp1 =
      new BallObject(TplayerPos1, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp1);

  glm::vec2 TplayerPos2 = glm::vec2(
      this->Width * 0.75f, this->Height * 0.25f - GameConstants::PLAYER_RADIUS);  // defender1
  BallObject* Tp2 =
      new BallObject(TplayerPos2, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp2);

  glm::vec2 TplayerPos3 = glm::vec2(
      this->Width * 0.75f, this->Height * 0.75f - GameConstants::PLAYER_RADIUS);  // defender2
  BallObject* Tp3 =
      new BallObject(TplayerPos3, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp3);

  glm::vec2 TplayerPos4 = glm::vec2(
      this->Width * 0.6f, this->Height * 0.15f - GameConstants::PLAYER_RADIUS);  // midfielder1
  BallObject* Tp4 =
      new BallObject(TplayerPos4, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp4);

  glm::vec2 TplayerPos5 = glm::vec2(
      this->Width * 0.6f, this->Height * 0.5f - GameConstants::PLAYER_RADIUS);  // midfielder2
  BallObject* Tp5 =
      new BallObject(TplayerPos5, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp5);

  glm::vec2 TplayerPos6 = glm::vec2(
      this->Width * 0.6f, this->Height * 0.85f - GameConstants::PLAYER_RADIUS);  // midfielder3
  BallObject* Tp6 =
      new BallObject(TplayerPos6, GameConstants::PLAYER_RADIUS,
                     glm::vec2(GameConstants::PLAYER_VELOCITY, GameConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp6);

  // Ball
  glm::vec2 ballPos = playerPos1 + glm::vec2(GameConstants::BALL_RADIUS * 2.0f,
                                             GameConstants::GameConstants::PLAYER_RADIUS -
                                                 GameConstants::BALL_RADIUS);
  ball = new BallObject(ballPos, GameConstants::BALL_RADIUS, GameConstants::INITIAL_BALL_VELOCITY,
                        resource_manager.GetTexture("face"), true);
}

Point2D screenToRRTX(const glm::vec2& pos, unsigned int width, unsigned int height) {
  return {(pos.x / width) * 12.0 - 6.0, (pos.y / height) * 9.0 - 4.5};
}

glm::vec2 rrtxToScreen(const Point2D& p, unsigned int width, unsigned int height) {
  return {(float)((p.x + 6.0) / 12.0 * width), (float)((p.y + 4.5) / 9.0 * height)};
}

void Game::Update(float dt) {
  ball->Move(dt, this->Width, this->Height);

  BallObject* movableBot = nullptr;
  for (BallObject* p : team1_players)
    if (!p->lock) movableBot = p;
  for (BallObject* p : team2_players)
    if (!p->lock) movableBot = p;

  if (movableBot && this->State == GAME_ACTIVE) {
    bool ballIsStuckToMe = ball->Stuck && (glm::length(movableBot->Position - ball->Position) <
                                           movableBot->Radius * 4.0f);
    if (!ballIsStuckToMe) {
      RRTX rrtx_planner;
      rrtx_planner.setStart(screenToRRTX(movableBot->Position, this->Width, this->Height));
      rrtx_planner.setGoal(screenToRRTX(ball->Position, this->Width, this->Height));

      std::vector<Obstacle> obstacles;
      int id_counter = 0;
      auto add_obstacle = [&](BallObject* p) {
        if (p->lock) {
          Point2D pos = screenToRRTX(p->Position, this->Width, this->Height);
          double radius = (GameConstants::PLAYER_RADIUS / this->Width) * 12.0;
          obstacles.push_back({id_counter++, pos, radius});
        }
      };
      for (BallObject* p : team1_players) add_obstacle(p);
      for (BallObject* p : team2_players) add_obstacle(p);

      rrtx_planner.setObstacles(obstacles);

      std::vector<Point2D> path = rrtx_planner.plan();
      if (path.size() > 1) {
        glm::vec2 target = rrtxToScreen(path[1], this->Width, this->Height);
        glm::vec2 diff = target - movableBot->Position;
        if (glm::length(diff) > 1.0f) {
          glm::vec2 dir = glm::normalize(diff);
          float velocity = GameConstants::PLAYER_VELOCITY * dt;
          movableBot->Position += dir * velocity;

          if (movableBot->Position.x < 50.0f) movableBot->Position.x = 50.0f;
          if (movableBot->Position.x > this->Width - movableBot->Size.x - 50.0f)
            movableBot->Position.x = this->Width - movableBot->Size.x - 50.0f;
          if (movableBot->Position.y < 30.0f) movableBot->Position.y = 30.0f;
          if (movableBot->Position.y > this->Height - movableBot->Size.y - 30.0f)
            movableBot->Position.y = this->Height - movableBot->Size.y - 30.0f;
        }
      }
    }
  }

  this->DoCollisions();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  Render();

  glfwSwapBuffers(game_window.gl_window);
}



void Game::ProcessInput(float dt) {
  glfwPollEvents();

  auto handleCollision = [&](BallObject* Player) {
    bool isStuckToThisPlayer =
        ball->Stuck && (glm::length(Player->Position - ball->Position) < Player->Radius * 4.0f);
    if (!Player->lock) {
      if (this->State == GAME_ACTIVE) {
        float velocity = GameConstants::PLAYER_VELOCITY * dt;
        if (keys[GLFW_KEY_A]) {
          if (Player->Position.x >= 50.0f) {
            Player->Position.x -= velocity;
            if (isStuckToThisPlayer) ball->Position.x -= velocity;
          }
        }
        if (keys[GLFW_KEY_D]) {
          if (Player->Position.x <= this->Width - Player->Size.x - 50.0f) {
            Player->Position.x += velocity;
            if (isStuckToThisPlayer) ball->Position.x += velocity;
          }
        }

        if (keys[GLFW_KEY_W]) {
          if (Player->Position.y >= 30.0f) {
            Player->Position.y -= velocity;
            if (isStuckToThisPlayer) ball->Position.y -= velocity;
          }
        }
        if (keys[GLFW_KEY_S]) {
          if (Player->Position.y <= this->Height - Player->Size.y - 30.0f) {
            Player->Position.y += velocity;
            if (isStuckToThisPlayer) ball->Position.y += velocity;
          }
        }

        if (keys[GLFW_KEY_K]) ball->Stuck = false;
      }
    } else if (Player == team2_players[0]) {
      if (this->State == GAME_ACTIVE) {
        float velocity = GameConstants::PLAYER_VELOCITY * dt;
        if (keys[GLFW_KEY_LEFT]) {
          if (Player->Position.x >= 50.0f) {
            Player->Position.x -= velocity;
            if (isStuckToThisPlayer) ball->Position.x -= velocity;
          }
        }
        if (keys[GLFW_KEY_RIGHT]) {
          if (Player->Position.x <= this->Width - Player->Size.x - 50.0f) {
            Player->Position.x += velocity;
            if (isStuckToThisPlayer) ball->Position.x += velocity;
          }
        }

        if (keys[GLFW_KEY_UP]) {
          if (Player->Position.y >= 30.0f) {
            Player->Position.y -= velocity;
            if (isStuckToThisPlayer) ball->Position.y -= velocity;
          }
        }
        if (keys[GLFW_KEY_DOWN]) {
          if (Player->Position.y <= this->Height - Player->Size.y - 30.0f) {
            Player->Position.y += velocity;
            if (isStuckToThisPlayer) ball->Position.y += velocity;
          }
        }
      }
    }
  };

  for (BallObject* p : team1_players) handleCollision(p);
  for (BallObject* p : team2_players) handleCollision(p);
}

void Game::Render() {
  if (this->State == GAME_ACTIVE) {
    static Texture2D texture = resource_manager.GetTexture("background");
    renderer->DrawSprite(texture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height),
                         0.0f);
    this->Levels[this->Level].Draw(*renderer);

    for (BallObject* p : team1_players) p->Draw(*renderer);
    for (BallObject* p : team2_players) p->Draw(*renderer);
    ball->Draw(*renderer);
  }
}

Direction VectorDirection(glm::vec2 target) {
  glm::vec2 compass[] = {
      glm::vec2(0.0f, 1.0f),   // up
      glm::vec2(1.0f, 0.0f),   // right
      glm::vec2(0.0f, -1.0f),  // down
      glm::vec2(-1.0f, 0.0f)   // left
  };
  float max = 0.0f;
  unsigned int best_match = -1;
  for (unsigned int i = 0; i < 4; i++) {
    float dot_product = glm::dot(glm::normalize(target), compass[i]);
    if (dot_product > max) {
      max = dot_product;
      best_match = i;
    }
  }
  return (Direction)best_match;
}

Collision CheckCollision(BallObject& one, BallObject& two)  // Circle - Circle collision
{
  glm::vec2 centerOne(one.Position + one.Radius);
  glm::vec2 centerTwo(two.Position + two.Radius);
  glm::vec2 difference = centerOne - centerTwo;
  float distance = glm::length(difference);
  float radiiSum = one.Radius + two.Radius;

  if (distance <= radiiSum) {
    if ((centerTwo.x + GameConstants::PLAYER_RADIUS - GameConstants::Stuckerror) <=
        ((one.Position).x)) {
      one.Stuck = true;
    }
    return std::make_tuple(true, VectorDirection(difference), difference);
  }

  else
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions() {
  auto handleCollision = [&](BallObject* player) {
    Collision result = CheckCollision(*ball, *player);
    if (!ball->Stuck && std::get<0>(result)) {
      float centerBoard = player->Position.x + GameConstants::PLAYER_RADIUS;
      float distance = (ball->Position.x + ball->Radius) - centerBoard;
      float percentage = distance / GameConstants::PLAYER_RADIUS;
      float strength = 2.0f;

      glm::vec2 oldVelocity = ball->Velocity;
      ball->Velocity.x = GameConstants::INITIAL_BALL_VELOCITY.x * percentage * strength;
      ball->Velocity = glm::normalize(ball->Velocity) * glm::length(oldVelocity);
      ball->Velocity.y = -1.0f * abs(ball->Velocity.y);
    }
  };

  for (BallObject* p : team1_players) handleCollision(p);
  for (BallObject* p : team2_players) handleCollision(p);
}