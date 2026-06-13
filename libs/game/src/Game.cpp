#include "Game.h"

#ifndef SHADER_DIR
#define SHADER_DIR ""
#endif

#ifndef TEXTURE_DIR
#define TEXTURE_DIR ""
#endif

#ifndef LEVEL_DIR
#define LEVEL_DIR ""
#endif

#include "RRTX.h"
#include "Obstacle.h"
#include "SystemCoordinates.h"

Game::Game() : State(GAME_ACTIVE) {
  this->Width = SystemConstants::SCREEN_WIDTH;
  this->Height = SystemConstants::SCREEN_HEIGHT;
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
  glm::vec2 playerPos1 =
      glm::vec2(this->Width * 0.125f,
                this->Height * 0.5f - SystemConstants::PLAYER_RADIUS);  // keeper
  BallObject* p1 =
      new BallObject(playerPos1, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), false);
  team1_players.push_back(p1);

  glm::vec2 playerPos2 =
      glm::vec2(this->Width * 0.25f,
                this->Height * 0.25f - SystemConstants::PLAYER_RADIUS);  // defender1
  BallObject* p2 =
      new BallObject(playerPos2, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p2);

  glm::vec2 playerPos3 =
      glm::vec2(this->Width * 0.25f,
                this->Height * 0.75f - SystemConstants::PLAYER_RADIUS);  // defender2
  BallObject* p3 =
      new BallObject(playerPos3, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p3);

  glm::vec2 playerPos4 =
      glm::vec2(this->Width * 0.4f,
                this->Height * 0.15f - SystemConstants::PLAYER_RADIUS);  // midfielder1
  BallObject* p4 =
      new BallObject(playerPos4, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p4);

  glm::vec2 playerPos5 =
      glm::vec2(this->Width * 0.4f,
                this->Height * 0.5f - SystemConstants::PLAYER_RADIUS);  // midfielder2
  BallObject* p5 =
      new BallObject(playerPos5, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p5);

  glm::vec2 playerPos6 =
      glm::vec2(this->Width * 0.4f,
                this->Height * 0.85f - SystemConstants::PLAYER_RADIUS);  // midfielder3
  BallObject* p6 =
      new BallObject(playerPos6, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team1_players.push_back(p6);

  // Team 2 players
  glm::vec2 TplayerPos1 =
      glm::vec2(this->Width * 0.875f,
                this->Height * 0.5f - SystemConstants::PLAYER_RADIUS);  // keeper
  BallObject* Tp1 =
      new BallObject(TplayerPos1, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp1);

  glm::vec2 TplayerPos2 =
      glm::vec2(this->Width * 0.75f,
                this->Height * 0.25f - SystemConstants::PLAYER_RADIUS);  // defender1
  BallObject* Tp2 =
      new BallObject(TplayerPos2, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp2);

  glm::vec2 TplayerPos3 =
      glm::vec2(this->Width * 0.75f,
                this->Height * 0.75f - SystemConstants::PLAYER_RADIUS);  // defender2
  BallObject* Tp3 =
      new BallObject(TplayerPos3, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp3);

  glm::vec2 TplayerPos4 =
      glm::vec2(this->Width * 0.6f,
                this->Height * 0.15f - SystemConstants::PLAYER_RADIUS);  // midfielder1
  BallObject* Tp4 =
      new BallObject(TplayerPos4, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp4);

  glm::vec2 TplayerPos5 =
      glm::vec2(this->Width * 0.6f,
                this->Height * 0.5f - SystemConstants::PLAYER_RADIUS);  // midfielder2
  BallObject* Tp5 =
      new BallObject(TplayerPos5, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp5);

  glm::vec2 TplayerPos6 =
      glm::vec2(this->Width * 0.6f,
                this->Height * 0.85f - SystemConstants::PLAYER_RADIUS);  // midfielder3
  BallObject* Tp6 =
      new BallObject(TplayerPos6, SystemConstants::PLAYER_RADIUS,
                     glm::vec2(SystemConstants::PLAYER_VELOCITY, SystemConstants::PLAYER_VELOCITY),
                     resource_manager.GetTexture("robot"), true);
  team2_players.push_back(Tp6);

  // Ball
  glm::vec2 ballPos = playerPos1 + glm::vec2(SystemConstants::PLAYER_RADIUS * 2.0f + 5.0f,
                                             SystemConstants::PLAYER_RADIUS -
                                                 SystemConstants::BALL_RADIUS);
  ball = new BallObject(ballPos, SystemConstants::BALL_RADIUS,
                        glm::vec2(SystemConstants::INITIAL_BALL_VELOCITY.first,
                                  SystemConstants::INITIAL_BALL_VELOCITY.second),
                        resource_manager.GetTexture("face"), true);
}

std::vector<Point2D> Game::Plan(std::pair<double, double> start, std::pair<double, double> goal,
                                std::vector<Obstacle> obstacles) {
  switch (current_algo) {
    case AlgoName::RRTX:
      return rrtx_planner.PlanningStep(start, goal, obstacles);
    case AlgoName::DUMMY:
      return std::vector<Point2D>();
    default:
      std::cout << "[Game::Plan]: Algo doesn't exist" << std::endl;
      return std::vector<Point2D>();
  }
}

void Game::UpdateSimulation(double dt) {
  ball->Move(dt, this->Width, this->Height);

  BallObject* movableBot = nullptr;
  for (BallObject* p : team1_players)
    if (!p->lock) movableBot = p;
  for (BallObject* p : team2_players)
    if (!p->lock) movableBot = p;

  if (movableBot && this->State == GAME_ACTIVE) {
    bool ballIsStuckToMe = ball->Owner == movableBot;
    if (!ballIsStuckToMe) {
      // 1. Start, 2. Goal, 3. Obstacles
      std::vector<Obstacle> obstacles;
      int id_counter = 0;
      auto add_obstacle = [&](BallObject* p) {
        if (p->lock) {
          Point2D pos = screenToRRTX(p->Position.x, p->Position.y);
          double radius = ((SystemConstants::PLAYER_RADIUS + SystemConstants::OBSTACLE_TOLERANCE) /
                           SystemConstants::SCREEN_WIDTH) *
                          12.0;
          obstacles.push_back({id_counter++, pos, radius});
        }
      };
      for (BallObject* p : team1_players) add_obstacle(p);
      for (BallObject* p : team2_players) add_obstacle(p);

      // std::pair<double, double> start(movableBot->Position.x, movableBot->Position.y);
      // std::pair<double, double> goal(ball->Position.x, ball->Position.y);
      std::pair<double, double> start =
          screenToRRTX(movableBot->Position.x, movableBot->Position.y);
      std::pair<double, double> goal = screenToRRTX(ball->Position.x, ball->Position.y);

      std::vector<Point2D> path = Plan(start, goal, obstacles);

      if (path.size() > 1) {
        std::pair<double, double> target_ =
            rrtxToScreen(std::pair<double, double>(path[1].x, path[1].y));
        glm::vec2 target(target_.first, target_.second);
        glm::vec2 diff = target - movableBot->Position;
        if (glm::length(diff) > 1.0f) {
          // Determine if target changed significantly to regenerate profile
          if (glm::length(target - movableBot->current_target) > 5.0f) {
            movableBot->current_target = target;
            Motion::Point m_start{movableBot->Position.x, movableBot->Position.y};
            Motion::Point m_end{target.x, target.y};
            
            movableBot->currentProfile = movableBot->motionLibrary.generateProfile(
                m_start, m_end, glm::radians(movableBot->Rotation),
                movableBot->current_velocities
            );
            movableBot->current_segment_time = 0.0;
          }

          movableBot->current_segment_time += dt;
          movableBot->current_velocities = movableBot->motionLibrary.getVelocityState(
              movableBot->currentProfile, movableBot->current_segment_time);

          movableBot->Position.x += movableBot->current_velocities.vx * dt;
          movableBot->Position.y += movableBot->current_velocities.vy * dt;
          movableBot->Rotation += glm::degrees(movableBot->current_velocities.vtheta * dt);

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

void Game::Update(double dt) {
  // struct maintained for future
  UpdateSimulation(dt);
}

void Game::ProcessInput(double dt) {
  glfwPollEvents();

  auto handleCollision = [&](BallObject* Player) {
    bool isStuckToThisPlayer = ball->Owner == Player;
    if (!Player->lock) {
      if (this->State == GAME_ACTIVE) {
        float velocity = SystemConstants::PLAYER_VELOCITY * dt;
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

        float rotation_velocity = SystemConstants::PLAYER_ROTATION_VELOCITY * dt;
        float rot_change = 0.0f;
        if (keys[GLFW_KEY_Q]) {
          rot_change -= rotation_velocity;
        }
        if (keys[GLFW_KEY_E]) {
          rot_change += rotation_velocity;
        }

        if (rot_change != 0.0f) {
          Player->Rotation += rot_change;
          if (isStuckToThisPlayer) {
            glm::vec2 player_center = Player->Position + Player->Radius;
            glm::vec2 ball_center = ball->Position + ball->Radius;
            glm::vec2 diff = ball_center - player_center;
            
            float angle = glm::radians(rot_change);
            float cos_a = cos(angle);
            float sin_a = sin(angle);
            
            glm::vec2 new_diff(
              diff.x * cos_a - diff.y * sin_a,
              diff.x * sin_a + diff.y * cos_a
            );
            
            ball->Position = player_center + new_diff - ball->Radius;
          }
        }

        if (keys[GLFW_KEY_K]) {
          if (ball->Owner == Player) {
            glm::vec2 face_dir(cos(glm::radians(Player->Rotation)), sin(glm::radians(Player->Rotation)));
            ball->Velocity = face_dir * 500.0f;
            ball->Owner = nullptr;
          }
        }
      }
    } else if (Player == team2_players[0]) {
      if (this->State == GAME_ACTIVE) {
        float velocity = SystemConstants::PLAYER_VELOCITY * dt;
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

Collision CheckCollision(BallObject& one,
                         BallObject& two)  // Circle - Circle collision
{
  glm::vec2 centerOne(one.Position + one.Radius);
  glm::vec2 centerTwo(two.Position + two.Radius);
  glm::vec2 difference = centerOne - centerTwo;
  float distance = glm::length(difference);
  float radiiSum = one.Radius + two.Radius;

  if (distance <= radiiSum) {
    glm::vec2 face_dir(cos(glm::radians(two.Rotation)), sin(glm::radians(two.Rotation)));
    float projected_dist = glm::dot(difference, face_dir);
    if (projected_dist >= radiiSum - SystemConstants::Stuckerror) {
      if (one.Owner == nullptr) {
        one.Owner = &two;
      }
    }
    return std::make_tuple(true, VectorDirection(difference), difference);
  }

  else
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions() {
  auto handleCollision = [&](BallObject* player) {
    Collision result = CheckCollision(*ball, *player);
    if (std::get<0>(result)) {
      // Resolve overlap so ball cannot enter bot's outline
      glm::vec2 diff = std::get<2>(result);
      float dist = glm::length(diff);
      float penetration = (ball->Radius + player->Radius) - dist;
      if (dist > 0.0f && penetration > 0.0f) {
        ball->Position += glm::normalize(diff) * penetration;
      }

      // Bounce logic if not caught
      if (ball->Owner == nullptr) {
        if (glm::length(ball->Velocity) > 0.0f && dist > 0.0f) {
          glm::vec2 normal = glm::normalize(diff);
          float velocityDotNormal = glm::dot(ball->Velocity, normal);
          
          // Only bounce if the ball is actually moving towards the bot
          if (velocityDotNormal < 0.0f) {
            ball->Velocity = ball->Velocity - 2.0f * velocityDotNormal * normal;
            ball->Velocity *= SystemConstants::BALL_RESTITUTION;
          }
        }
      }
    }
  };

  auto checkBotBotCollision = [&](BallObject* p1, BallObject* p2) {
    if (p1 == p2) return;
    glm::vec2 c1 = p1->Position + p1->Radius;
    glm::vec2 c2 = p2->Position + p2->Radius;
    glm::vec2 diff = c1 - c2;
    float dist = glm::length(diff);
    float radiiSum = p1->Radius + p2->Radius;
    if (dist < radiiSum && dist > 0.0f) {
        float penetration = radiiSum - dist;
        glm::vec2 normal = glm::normalize(diff);
        if (!p1->lock && p2->lock) {
            p1->Position += normal * penetration;
        } else if (p1->lock && !p2->lock) {
            p2->Position -= normal * penetration;
        } else if (!p1->lock && !p2->lock) {
            p1->Position += normal * (penetration / 2.0f);
            p2->Position -= normal * (penetration / 2.0f);
        }
    }
  };

  std::vector<BallObject*> all_players;
  all_players.insert(all_players.end(), team1_players.begin(), team1_players.end());
  all_players.insert(all_players.end(), team2_players.begin(), team2_players.end());

  for (size_t i = 0; i < all_players.size(); i++) {
    for (size_t j = i + 1; j < all_players.size(); j++) {
      checkBotBotCollision(all_players[i], all_players[j]);
    }
  }

  for (BallObject* p : team1_players) handleCollision(p);
  for (BallObject* p : team2_players) handleCollision(p);
}

void Game::SetCurrentAlgo(AlgoName algo) { current_algo = algo; }

const std::vector<BallObject*>& Game::GetTeam1Players() const { return team1_players; }