#include "game.h"
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

const unsigned int Team1 = 6;
const unsigned int Team2 = 6;
int SelectedPlayer = 0;

float Stuckerror = 2.0f;
SpriteRenderer* Renderer;
std::vector<BallObject*> Team1Players;
std::vector<BallObject*> Team2Players;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {}

Game::~Game() {}

void Game::Cleanup() {
  delete Renderer;
  Renderer = nullptr;

  for (BallObject* p : Team1Players) delete p;
  Team1Players.clear();

  for (BallObject* p : Team2Players) delete p;
  Team2Players.clear();

  delete Ball;
  Ball = nullptr;
}

void Game::Init() {
  ResourceManager::LoadShader(SHADER_DIR "sprite.vs", SHADER_DIR "sprite.fs", nullptr, "sprite");
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

  ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
  ResourceManager::LoadTexture(TEXTURE_DIR "background.jpg", false, "background");
  ResourceManager::LoadTexture(TEXTURE_DIR "awesomeface.png", true, "face");
  ResourceManager::LoadTexture(TEXTURE_DIR "robot.png", true, "robot");

  static Shader shader = ResourceManager::GetShader("sprite");
  Renderer = new SpriteRenderer(shader);

  GameLevel one;
  one.Load(LEVEL_DIR "sim.lvl", this->Width, this->Height / 2);

  this->Levels.push_back(one);
  this->Level = 0;

  // Team 1 players
  glm::vec2 playerPos1 =
      glm::vec2(this->Width * 0.125f, this->Height * 0.5f - PLAYER_RADIUS);  // keeper
  BallObject* p1 =
      new BallObject(playerPos1, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), false);
  Team1Players.push_back(p1);

  glm::vec2 playerPos2 =
      glm::vec2(this->Width * 0.25f, this->Height * 0.25f - PLAYER_RADIUS);  // defender1
  BallObject* p2 =
      new BallObject(playerPos2, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team1Players.push_back(p2);

  glm::vec2 playerPos3 =
      glm::vec2(this->Width * 0.25f, this->Height * 0.75f - PLAYER_RADIUS);  // defender2
  BallObject* p3 =
      new BallObject(playerPos3, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team1Players.push_back(p3);

  glm::vec2 playerPos4 =
      glm::vec2(this->Width * 0.4f, this->Height * 0.15f - PLAYER_RADIUS);  // midfielder1
  BallObject* p4 =
      new BallObject(playerPos4, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team1Players.push_back(p4);

  glm::vec2 playerPos5 =
      glm::vec2(this->Width * 0.4f, this->Height * 0.5f - PLAYER_RADIUS);  // midfielder2
  BallObject* p5 =
      new BallObject(playerPos5, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team1Players.push_back(p5);

  glm::vec2 playerPos6 =
      glm::vec2(this->Width * 0.4f, this->Height * 0.85f - PLAYER_RADIUS);  // midfielder3
  BallObject* p6 =
      new BallObject(playerPos6, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team1Players.push_back(p6);

  // Team 2 players
  glm::vec2 TplayerPos1 =
      glm::vec2(this->Width * 0.875f, this->Height * 0.5f - PLAYER_RADIUS);  // keeper
  BallObject* Tp1 =
      new BallObject(TplayerPos1, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp1);

  glm::vec2 TplayerPos2 =
      glm::vec2(this->Width * 0.75f, this->Height * 0.25f - PLAYER_RADIUS);  // defender1
  BallObject* Tp2 =
      new BallObject(TplayerPos2, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp2);

  glm::vec2 TplayerPos3 =
      glm::vec2(this->Width * 0.75f, this->Height * 0.75f - PLAYER_RADIUS);  // defender2
  BallObject* Tp3 =
      new BallObject(TplayerPos3, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp3);

  glm::vec2 TplayerPos4 =
      glm::vec2(this->Width * 0.6f, this->Height * 0.15f - PLAYER_RADIUS);  // midfielder1
  BallObject* Tp4 =
      new BallObject(TplayerPos4, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp4);

  glm::vec2 TplayerPos5 =
      glm::vec2(this->Width * 0.6f, this->Height * 0.5f - PLAYER_RADIUS);  // midfielder2
  BallObject* Tp5 =
      new BallObject(TplayerPos5, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp5);

  glm::vec2 TplayerPos6 =
      glm::vec2(this->Width * 0.6f, this->Height * 0.85f - PLAYER_RADIUS);  // midfielder3
  BallObject* Tp6 =
      new BallObject(TplayerPos6, PLAYER_RADIUS, glm::vec2(PLAYER_VELOCITY, PLAYER_VELOCITY),
                     ResourceManager::GetTexture("robot"), true);
  Team2Players.push_back(Tp6);

  // Ball
  glm::vec2 ballPos = playerPos1 + glm::vec2(BALL_RADIUS * 2.0f, PLAYER_RADIUS - BALL_RADIUS);
  Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                        ResourceManager::GetTexture("face"), true);
}

Point2D screenToRRTX(const glm::vec2& pos, unsigned int width, unsigned int height) {
  return {(pos.x / width) * 12.0 - 6.0, (pos.y / height) * 9.0 - 4.5};
}

glm::vec2 rrtxToScreen(const Point2D& p, unsigned int width, unsigned int height) {
  return {(float)((p.x + 6.0) / 12.0 * width), (float)((p.y + 4.5) / 9.0 * height)};
}

void Game::Update(float dt) {
  Ball->Move(dt, this->Width, this->Height);

  BallObject* movableBot = nullptr;
  for (BallObject* p : Team1Players)
    if (!p->lock) movableBot = p;
  for (BallObject* p : Team2Players)
    if (!p->lock) movableBot = p;

  if (movableBot && this->State == GAME_ACTIVE) {
    bool ballIsStuckToMe = Ball->Stuck && (glm::length(movableBot->Position - Ball->Position) <
                                           movableBot->Radius * 4.0f);
    if (!ballIsStuckToMe) {
      RRTX rrtx_planner;
      rrtx_planner.setStart(screenToRRTX(movableBot->Position, this->Width, this->Height));
      rrtx_planner.setGoal(screenToRRTX(Ball->Position, this->Width, this->Height));

      std::vector<Obstacle> obstacles;
      int id_counter = 0;
      auto add_obstacle = [&](BallObject* p) {
        if (p->lock) {
          Point2D pos = screenToRRTX(p->Position, this->Width, this->Height);
          double radius = (PLAYER_RADIUS / this->Width) * 12.0;
          obstacles.push_back({id_counter++, pos, radius});
        }
      };
      for (BallObject* p : Team1Players) add_obstacle(p);
      for (BallObject* p : Team2Players) add_obstacle(p);

      rrtx_planner.setObstacles(obstacles);

      std::vector<Point2D> path = rrtx_planner.plan();
      if (path.size() > 1) {
        glm::vec2 target = rrtxToScreen(path[1], this->Width, this->Height);
        glm::vec2 diff = target - movableBot->Position;
        if (glm::length(diff) > 1.0f) {
          glm::vec2 dir = glm::normalize(diff);
          float velocity = PLAYER_VELOCITY * dt;
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
}

void Game::ProcessInput(float dt) {
  auto handleCollision = [&](BallObject* Player) {
    bool isStuckToThisPlayer =
        Ball->Stuck && (glm::length(Player->Position - Ball->Position) < Player->Radius * 4.0f);
    if (!Player->lock) {
      if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_A]) {
          if (Player->Position.x >= 50.0f) {
            Player->Position.x -= velocity;
            if (isStuckToThisPlayer) Ball->Position.x -= velocity;
          }
        }
        if (this->Keys[GLFW_KEY_D]) {
          if (Player->Position.x <= this->Width - Player->Size.x - 50.0f) {
            Player->Position.x += velocity;
            if (isStuckToThisPlayer) Ball->Position.x += velocity;
          }
        }

        if (this->Keys[GLFW_KEY_W]) {
          if (Player->Position.y >= 30.0f) {
            Player->Position.y -= velocity;
            if (isStuckToThisPlayer) Ball->Position.y -= velocity;
          }
        }
        if (this->Keys[GLFW_KEY_S]) {
          if (Player->Position.y <= this->Height - Player->Size.y - 30.0f) {
            Player->Position.y += velocity;
            if (isStuckToThisPlayer) Ball->Position.y += velocity;
          }
        }

        if (this->Keys[GLFW_KEY_K]) Ball->Stuck = false;
      }
    } else if (Player == Team2Players[0]) {
      if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_LEFT]) {
          if (Player->Position.x >= 50.0f) {
            Player->Position.x -= velocity;
            if (isStuckToThisPlayer) Ball->Position.x -= velocity;
          }
        }
        if (this->Keys[GLFW_KEY_RIGHT]) {
          if (Player->Position.x <= this->Width - Player->Size.x - 50.0f) {
            Player->Position.x += velocity;
            if (isStuckToThisPlayer) Ball->Position.x += velocity;
          }
        }

        if (this->Keys[GLFW_KEY_UP]) {
          if (Player->Position.y >= 30.0f) {
            Player->Position.y -= velocity;
            if (isStuckToThisPlayer) Ball->Position.y -= velocity;
          }
        }
        if (this->Keys[GLFW_KEY_DOWN]) {
          if (Player->Position.y <= this->Height - Player->Size.y - 30.0f) {
            Player->Position.y += velocity;
            if (isStuckToThisPlayer) Ball->Position.y += velocity;
          }
        }
      }
    }
  };

  for (BallObject* p : Team1Players) handleCollision(p);
  for (BallObject* p : Team2Players) handleCollision(p);
}

void Game::Render() {
  if (this->State == GAME_ACTIVE) {
    static Texture2D texture = ResourceManager::GetTexture("background");
    Renderer->DrawSprite(texture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height),
                         0.0f);
    this->Levels[this->Level].Draw(*Renderer);

    for (BallObject* p : Team1Players) p->Draw(*Renderer);
    for (BallObject* p : Team2Players) p->Draw(*Renderer);
    Ball->Draw(*Renderer);
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
    if ((centerTwo.x + PLAYER_RADIUS - Stuckerror) <= Ball->Position.x) {
      Ball->Stuck = true;
    }
    return std::make_tuple(true, VectorDirection(difference), difference);
  }

  else
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions() {
  auto handleCollision = [&](BallObject* player) {
    Collision result = CheckCollision(*Ball, *player);
    if (!Ball->Stuck && std::get<0>(result)) {
      float centerBoard = player->Position.x + PLAYER_RADIUS;
      float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
      float percentage = distance / PLAYER_RADIUS;
      float strength = 2.0f;

      glm::vec2 oldVelocity = Ball->Velocity;
      Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
      Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
      Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
  };

  for (BallObject* p : Team1Players) handleCollision(p);
  for (BallObject* p : Team2Players) handleCollision(p);
}