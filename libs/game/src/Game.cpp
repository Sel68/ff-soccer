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

#include "GameConfig.h"
#include "Obstacle.h"
#include "RRTX.h"
#include "SystemCoordinates.h"

Game::Game() : state(GAME_ACTIVE) {
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
  if (renderer == nullptr) return;
  std::cout << "[INFO] [Game]: Shutting down and cleaning up memory..." << std::endl;
  delete renderer;
  renderer = nullptr;

  for (GameObject* p : team1_players) delete p;
  team1_players.clear();

  for (GameObject* p : team2_players) delete p;
  team2_players.clear();

  delete ball;
  ball = nullptr;
}

void Game::ObjectPosInit() {
  // Team 1 players
  glm::vec2 playerPos1 =
      glm::vec2(SystemConstants::screen_width * 0.125f,
                SystemConstants::screen_height * 0.5f - GameConfig::player_radius);  // keeper
  GameObject* p1 =
      new GameObject(playerPos1, GameConfig::player_radius,
                     glm::vec2(GameConfig::player_velocity, GameConfig::player_velocity),
                     resource_manager.GetTexture("robot"), false);
  team1_players.push_back(p1);

  // Ball
  glm::vec2 ballPos = playerPos1 + glm::vec2(GameConfig::player_radius * 2.0f + 5.0f,
                                             GameConfig::player_radius - GameConfig::ball_radius);
  ball = new GameObject(
      ballPos, GameConfig::ball_radius,
      glm::vec2(GameConfig::initial_ball_velocity.first, GameConfig::initial_ball_velocity.second),
      resource_manager.GetTexture("face"), true);
}

void Game::Init() {
  std::cout << "          KEYBOARD CONTROLS             " << std::endl;
  std::cout << "  [J] - Toggle Algorithm Debug Logs     " << std::endl;
  std::cout << "  [M] - Toggle Motion Physics Logs      " << std::endl;
  std::cout << "  [N] - Toggle Strategy Logs            " << std::endl;
  std::cout << "  [K] - Kick Ball                       " << std::endl;
  std::cout << "  [L] - Toggle Auto / Manual Mode       " << std::endl;
  resource_manager.LoadShader(SHADER_DIR "sprite.vs", SHADER_DIR "sprite.fs", nullptr, "sprite");
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(SystemConstants::screen_width),
                 static_cast<float>(SystemConstants::screen_height), 0.0f, -1.0f, 1.0f);

  resource_manager.GetShader("sprite").Use().SetInteger("image", 0);
  resource_manager.GetShader("sprite").SetMatrix4("projection", projection);
  resource_manager.LoadTexture(TEXTURE_DIR "background.jpg", false, "background");
  resource_manager.LoadTexture(TEXTURE_DIR "awesomeface.png", true, "face");
  resource_manager.LoadTexture(TEXTURE_DIR "robot.png", true, "robot");

  ObjectPosInit();  // initiliase players and ball positions

  static Shader shader = resource_manager.GetShader("sprite");
  renderer = new SpriteRenderer(shader);
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
  HandleManualKick();

  ball->Move(dt, SystemConstants::screen_width, SystemConstants::screen_height);

  GameObject* movableBot = nullptr;
  for (GameObject* p : team1_players)
    if (!p->lock) movableBot = p;
  for (GameObject* p : team2_players)
    if (!p->lock) movableBot = p;

  if (movableBot && this->state == GAME_ACTIVE) {
    if (is_auto_mode)
      UpdateAutoStrategy(movableBot, dt);
    else
      UpdateManualMotion(movableBot, dt);
  }

  this->DoCollisions();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  Render();
  glfwSwapBuffers(game_window.gl_window);
}

void Game::Update(double dt) { UpdateSimulation(dt); }

void Game::ProcessInput(double dt, double posX, double posY, double theta) {
  glfwPollEvents();

  ProcessDebugKeys();
  ProcessMouseClickMovement();

  for (GameObject* p : team1_players) ProcessPlayerInput(p, dt, posX, posY, theta);
  for (GameObject* p : team2_players) ProcessPlayerInput(p, dt, posX, posY, theta);
}

void Game::Render() {
  if (this->state == GAME_ACTIVE) {
    static Texture2D texture = resource_manager.GetTexture("background");
    renderer->DrawSprite(texture, glm::vec2(0.0f, 0.0f),
                         glm::vec2(SystemConstants::screen_width, SystemConstants::screen_height),
                         0.0f);
    // this->Levels[this->level].Draw(*renderer);

    for (GameObject* p : team1_players) p->Draw(*renderer);
    for (GameObject* p : team2_players) p->Draw(*renderer);
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

Collision CheckCollision(GameObject& one,
                         GameObject& two)  // Circle - Circle collision
{
  glm::vec2 centerOne(one.position + one.radius);
  glm::vec2 centerTwo(two.position + two.radius);
  glm::vec2 difference = centerOne - centerTwo;
  float distance = glm::length(difference);
  float radiiSum = one.radius + two.radius;

  if (distance <= radiiSum) {
    glm::vec2 face_dir(cos(glm::radians(two.rotation)), sin(glm::radians(two.rotation)));
    float projected_dist = glm::dot(difference, face_dir);
    if (projected_dist >= radiiSum - GameConfig::stuck_error) {
      if (one.owner == nullptr) {
        one.owner = &two;
      }
    }
    return std::make_tuple(true, VectorDirection(difference), difference);
  }

  else
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions() {
  std::vector<GameObject*> all_players;
  all_players.insert(all_players.end(), team1_players.begin(), team1_players.end());
  all_players.insert(all_players.end(), team2_players.begin(), team2_players.end());

  for (size_t i = 0; i < all_players.size(); i++) {
    for (size_t j = i + 1; j < all_players.size(); j++) {
      HandleBotCollision(all_players[i], all_players[j]);
    }
  }

  for (GameObject* p : team1_players) HandleBallCollision(p);
  for (GameObject* p : team2_players) HandleBallCollision(p);
}

void Game::SetCurrentAlgo(AlgoName algo) { current_algo = algo; }

const std::vector<GameObject*>& Game::GetTeam1Players() const { return team1_players; }

void Game::HandleManualKick() {
  // kick signal is set/cleared by ProcessInput (K=1.0f, P=0.0f) and persists
  // across frames — PrepareRobotCommands reads it from p->kick each cycle.

  if (keys[GLFW_KEY_K] && ball->owner != nullptr) {
    glm::vec2 face_dir(cos(glm::radians(ball->owner->rotation)),
                       sin(glm::radians(ball->owner->rotation)));
    ball->velocity = face_dir * 500.0f;
    ball->owner = nullptr;
  }
}

void Game::UpdateAutoStrategy(GameObject* movableBot, double dt) {
  std::vector<Obstacle> obstacles;
  int id_counter = 0;
  auto add_obstacle = [&](GameObject* p) {
    if (p->lock) {
      Point2D pos = screenToRRTX(p->position.x, p->position.y);
      double radius = ((GameConfig::player_radius + GameConfig::obstacle_tolerance) /
                       SystemConstants::screen_width) *
                      12.0;
      obstacles.push_back({id_counter++, pos, radius});
    }
  };
  for (GameObject* p : team1_players) add_obstacle(p);
  for (GameObject* p : team2_players) add_obstacle(p);

  StrategyContext ctx;
  ctx.robot_pos = {movableBot->position.x, movableBot->position.y};
  ctx.robot_rotation = movableBot->rotation;
  ctx.ball_pos = {ball->position.x, ball->position.y};
  ctx.has_ball = (ball->owner == movableBot);
  ctx.goal_pos = {(double)SystemConstants::screen_width, SystemConstants::screen_height / 2.0};

  if (is_auto_mode) {
    StrategyResult strat_res = m_strategy.update(ctx);

    if (strat_res.kick && ball->owner == movableBot) {
      glm::vec2 face_dir(cos(glm::radians(movableBot->rotation)),
                         sin(glm::radians(movableBot->rotation)));
      ball->velocity = face_dir * 500.0f;
      ball->owner = nullptr;
    }

    std::pair<double, double> start = screenToRRTX(movableBot->position.x, movableBot->position.y);
    std::pair<double, double> goal =
        screenToRRTX(strat_res.target_pos.first, strat_res.target_pos.second);

    std::vector<Point2D> path = Plan(start, goal, obstacles);

    if (path.size() <= 1) {
      std::cout << "[ERROR] [Game]: RRTX returned an empty path! Robot is trapped." << std::endl;
    }

    if (path.size() > 1) {
      std::pair<double, double> target_ =
          rrtxToScreen(std::pair<double, double>(path[1].x, path[1].y));
      glm::vec2 target(target_.first, target_.second);
      glm::vec2 diff = target - movableBot->position;
      if (glm::length(diff) > 1.0f) {
        if (glm::length(target - movableBot->current_target) > 5.0f) {
          movableBot->current_target = target;
          Motion::Point m_start{movableBot->position.x, movableBot->position.y};
          Motion::Point m_end{target.x, target.y};

          movableBot->currentProfile = movableBot->motion_library.generateProfile(
              m_start, m_end, glm::radians(movableBot->rotation), movableBot->current_velocities);
          movableBot->current_segment_time = 0.0;
        }

        movableBot->current_segment_time += dt;
        movableBot->current_velocities = movableBot->motion_library.getVelocityState(
            movableBot->currentProfile, movableBot->current_segment_time);

        double dx = movableBot->current_velocities.vx * dt;
        double dy = movableBot->current_velocities.vy * dt;
        double dtheta = glm::degrees(movableBot->current_velocities.vtheta * dt);

        movableBot->position.x += dx;
        movableBot->position.y += dy;
        movableBot->rotation += dtheta;
        movableBot->rotation = std::fmod(movableBot->rotation, 360.0f);
        if (movableBot->rotation < 0.0f) movableBot->rotation += 360.0f;

        if (ball->owner == movableBot) {
          ball->position.x += dx;
          ball->position.y += dy;

          if (dtheta != 0.0f) {
            glm::vec2 ball_center = ball->position + ball->radius;
            glm::vec2 player_center = movableBot->position + movableBot->radius;
            glm::vec2 diff = ball_center - player_center;

            float angle = glm::radians(dtheta);
            glm::vec2 new_diff;
            new_diff.x = diff.x * cos(angle) - diff.y * sin(angle);
            new_diff.y = diff.x * sin(angle) + diff.y * cos(angle);

            ball->position = player_center + new_diff - ball->radius;
          }
        }

        if (movableBot->position.x < 50.0f) movableBot->position.x = 50.0f;
        if (movableBot->position.x > SystemConstants::screen_width - movableBot->size.x - 50.0f)
          movableBot->position.x = SystemConstants::screen_width - movableBot->size.x - 50.0f;
        if (movableBot->position.y < 30.0f) movableBot->position.y = 30.0f;
        if (movableBot->position.y > SystemConstants::screen_height - movableBot->size.y - 30.0f)
          movableBot->position.y = SystemConstants::screen_height - movableBot->size.y - 30.0f;
      }
    }
  }
}

void Game::UpdateManualMotion(GameObject* robot, double dt) {
  if (!robot->currentProfile.valid) return;

  robot->current_segment_time += dt;
  robot->current_velocities =
      robot->motion_library.getVelocityState(robot->currentProfile, robot->current_segment_time);

  double dx = robot->current_velocities.vx * dt;
  double dy = robot->current_velocities.vy * dt;
  double dtheta = glm::degrees(robot->current_velocities.vtheta * dt);

  robot->position.x += dx;
  robot->position.y += dy;
  robot->rotation += dtheta;
}

void Game::ProcessDebugKeys() {
  if (keys[GLFW_KEY_J] && !keys_processed[GLFW_KEY_J]) {
    static bool is_debug_mode = false;
    is_debug_mode = !is_debug_mode;
    keys_processed[GLFW_KEY_J] = true;
    rrtx_planner.setDebugMode(is_debug_mode);
    std::cout << "[INFO] [Game]: Algo Debug Mode " << (is_debug_mode ? "ON" : "OFF") << std::endl;
  }

  if (keys[GLFW_KEY_M] && !keys_processed[GLFW_KEY_M]) {
    static bool is_motion_debug_mode = false;
    is_motion_debug_mode = !is_motion_debug_mode;
    keys_processed[GLFW_KEY_M] = true;
    Motion::setDebugMode(is_motion_debug_mode);
    std::cout << "[INFO] [Game]: Motion Physics Debug Mode "
              << (is_motion_debug_mode ? "ON" : "OFF") << std::endl;
  }

  if (keys[GLFW_KEY_N] && !keys_processed[GLFW_KEY_N]) {
    static bool is_strat_debug_mode = false;
    is_strat_debug_mode = !is_strat_debug_mode;
    keys_processed[GLFW_KEY_N] = true;
    MainStrategy::setDebugMode(is_strat_debug_mode);
    std::cout << "[INFO] [Game]: Strategy Debug Mode " << (is_strat_debug_mode ? "ON" : "OFF")
              << std::endl;
  }

  if (keys[GLFW_KEY_L] && !keys_processed[GLFW_KEY_L]) {
    is_auto_mode = !is_auto_mode;
    keys_processed[GLFW_KEY_L] = true;
    std::cout << "[INFO] [Game]: Switched to " << (is_auto_mode ? "AUTO" : "MANUAL") << " mode."
              << std::endl;
    if (is_auto_mode) {
      for (GameObject* p : team1_players) {
        p->current_target = glm::vec2(-10000.0f, -10000.0f);
      }
      for (GameObject* p : team2_players) {
        p->current_target = glm::vec2(-10000.0f, -10000.0f);
      }
    }
  }
}

void Game::ProcessPlayerInput(GameObject* Player, double dt, double posX, double posY,
                              double theta) {
  glm::vec2 oldPosition = Player->position;
  float oldRotation = Player->rotation;

  bool isStuckToThisPlayer = ball->owner == Player;
  if (!Player->lock) {
    if (this->state == GAME_ACTIVE) {
      float velocity = GameConfig::player_velocity * dt;
      if (keys[GLFW_KEY_P]) {
        passive = true;
      } else if (keys[GLFW_KEY_O]) {
        passive = false;
      }

      if (passive) {
        Player->position.x = 50 + (posX + 1785) * 850 / 3570;
        Player->position.y = 30 + (1190 - posY) * 520 / 2380;
        Player->rotation = theta * 180 / 3.14;
        if (keys[GLFW_KEY_C]) {
          Player->charge = 1.0f;
        }
        if (keys[GLFW_KEY_V]) {
          Player->charge = 0.0f;
        }
        if (keys[GLFW_KEY_K]) {
          Player->kick = 1.0f;
        }
        if (keys[GLFW_KEY_I]) {
          Player->kick = 0.0f;
        }
      }

      else {
        if (keys[GLFW_KEY_A]) {
          if (Player->position.x >= 50.0f) {
            Player->position.x -= velocity;
            if (isStuckToThisPlayer) ball->position.x -= velocity;
          }
        }
        if (keys[GLFW_KEY_D]) {
          if (Player->position.x <= SystemConstants::screen_width - Player->size.x - 50.0f) {
            Player->position.x += velocity;
            if (isStuckToThisPlayer) ball->position.x += velocity;
          }
        }

        if (keys[GLFW_KEY_W]) {
          if (Player->position.y >= 30.0f) {
            Player->position.y -= velocity;
            if (isStuckToThisPlayer) ball->position.y -= velocity;
          }
        }
        if (keys[GLFW_KEY_S]) {
          if (Player->position.y <= SystemConstants::screen_height - Player->size.y - 30.0f) {
            Player->position.y += velocity;
            if (isStuckToThisPlayer) ball->position.y += velocity;
          }
        }
        if (keys[GLFW_KEY_C]) {
          Player->charge = 1.0f;
        }
        if (keys[GLFW_KEY_V]) {
          Player->charge = 0.0f;
        }
        if (keys[GLFW_KEY_K]) {
          Player->kick = 1.0f;
        }
        if (keys[GLFW_KEY_I]) {
          Player->kick = 0.0f;
        }

        // std::cout << "Player Position: (" << Player->position.x << ", " << Player->position.y
        // << ")" << std::endl;

        float rotation_velocity = GameConfig::player_rotation_velocity * dt;
        float rot_change = 0.0f;
        if (keys[GLFW_KEY_Q]) {
          rot_change -= rotation_velocity;
        }
        if (keys[GLFW_KEY_E]) {
          rot_change += rotation_velocity;
        }

        if (rot_change != 0.0f) {
          Player->rotation += rot_change;
          Player->rotation = std::fmod(Player->rotation, 360.0f);
          if (Player->rotation < 0.0f) Player->rotation += 360.0f;
          if (isStuckToThisPlayer) {
            glm::vec2 player_center = Player->position + Player->radius;
            glm::vec2 ball_center = ball->position + ball->radius;
            glm::vec2 diff = ball_center - player_center;

            float angle = glm::radians(rot_change);
            float cos_a = cos(angle);
            float sin_a = sin(angle);

            glm::vec2 new_diff(diff.x * cos_a - diff.y * sin_a, diff.x * sin_a + diff.y * cos_a);

            ball->position = player_center + new_diff - ball->radius;
          }
        }
      }
    }
  } else if (Player == team2_players[0]) {
    if (this->state == GAME_ACTIVE) {
      float velocity = GameConfig::player_velocity * dt;
      if (keys[GLFW_KEY_LEFT]) {
        if (Player->position.x >= 50.0f) {
          Player->position.x -= velocity;
          if (isStuckToThisPlayer) ball->position.x -= velocity;
        }
      }
      if (keys[GLFW_KEY_RIGHT]) {
        if (Player->position.x <= SystemConstants::screen_width - Player->size.x - 50.0f) {
          Player->position.x += velocity;
          if (isStuckToThisPlayer) ball->position.x += velocity;
        }
      }

      if (keys[GLFW_KEY_UP]) {
        if (Player->position.y >= 30.0f) {
          Player->position.y -= velocity;
          if (isStuckToThisPlayer) ball->position.y -= velocity;
        }
      }
      if (keys[GLFW_KEY_DOWN]) {
        if (Player->position.y <= SystemConstants::screen_height - Player->size.y - 30.0f) {
          Player->position.y += velocity;
          if (isStuckToThisPlayer) ball->position.y += velocity;
        }
      }
    }
  }

  if (dt > 0.0001) {
    Player->velocity = (Player->position - oldPosition) / static_cast<float>(dt);
    float rot_diff = Player->rotation - oldRotation;
    if (rot_diff > 180.0f) rot_diff -= 360.0f;
    else if (rot_diff < -180.0f) rot_diff += 360.0f;
    Player->angular_velocity = rot_diff / static_cast<float>(dt);
  } else {
    Player->velocity = glm::vec2(0.0f, 0.0f);
    Player->angular_velocity = 0.0f;
  }
}

void Game::ProcessMouseClickMovement() {
  // printing coordinates of the field where the mouse clicks
  if (glfwGetMouseButton(this->game_window.gl_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(this->game_window.gl_window, &xpos, &ypos);

    std::cout << "Mouse clicked at: (" << xpos << ", " << ypos << ")\n";
  }

  /*
  Handling the case when the mouse clicks in the manual mode, we use our custom
  bang bang profiler to move there.
  */
  static bool prevLeftClick = false;
  bool leftClick =
      glfwGetMouseButton(this->game_window.gl_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
  if (!prevLeftClick && leftClick && !is_auto_mode) {
    double xpos, ypos;
    glfwGetCursorPos(this->game_window.gl_window, &xpos, &ypos);

    GameObject* robot = team1_players[0];
    robot->current_target = glm::vec2(xpos, ypos);

    Motion::Point start{robot->position.x, robot->position.y};
    Motion::Point end{xpos, ypos};

    robot->currentProfile = robot->motion_library.generateProfile(
        start, end, glm::radians(robot->rotation), robot->current_velocities);
    robot->current_segment_time = 0.0f;
  }

  prevLeftClick = leftClick;
}

void Game::HandleBallCollision(GameObject* player) {
  Collision result = CheckCollision(*ball, *player);
  if (std::get<0>(result)) {
    glm::vec2 diff = std::get<2>(result);
    float dist = glm::length(diff);
    float penetration = (ball->radius + player->radius) - dist;
    if (dist > 0.0f && penetration > 0.0f) {
      ball->position += glm::normalize(diff) * penetration;
    }

    if (ball->owner == nullptr) {
      if (glm::length(ball->velocity) > 0.0f && dist > 0.0f) {
        glm::vec2 normal = glm::normalize(diff);
        float velocityDotNormal = glm::dot(ball->velocity, normal);

        if (velocityDotNormal < 0.0f) {
          ball->velocity = ball->velocity - 2.0f * velocityDotNormal * normal;
          ball->velocity *= GameConfig::ball_restitution;
        }
      }
    }
  }
}

void Game::HandleBotCollision(GameObject* p1, GameObject* p2) {
  if (p1 == p2) return;
  glm::vec2 c1 = p1->position + p1->radius;
  glm::vec2 c2 = p2->position + p2->radius;
  glm::vec2 diff = c1 - c2;
  float dist = glm::length(diff);
  float radiiSum = p1->radius + p2->radius;
  if (dist < radiiSum && dist > 0.0f) {
    float penetration = radiiSum - dist;
    glm::vec2 normal = glm::normalize(diff);
    if (!p1->lock && p2->lock) {
      p1->position += normal * penetration;
    } else if (p1->lock && !p2->lock) {
      p2->position -= normal * penetration;
    } else if (!p1->lock && !p2->lock) {
      p1->position += normal * (penetration / 2.0f);
      p2->position -= normal * (penetration / 2.0f);
    }
  }
}