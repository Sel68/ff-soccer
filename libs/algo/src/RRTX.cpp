#include <algorithm>
#include <chrono>
#include <random>

#include "RRTX.h"
#include "SystemConstants.h"

RRTX::RRTX(const RRTXConfig& config) : m_config(config) {}

RRTX::~RRTX() {}

std::vector<Point2D> RRTX::PlanningStep(std::pair<double, double> start,
                                        std::pair<double, double> goal,
                                        std::vector<Obstacle> obstacles) {
  setStart({start.first, start.second});
  setGoal({goal.first, goal.second});
  setObstacles(obstacles);
  return plan();
}

void RRTX::setGoal(const Point2D& goal) { m_goal = goal; }

void RRTX::setStart(const Point2D& start) { m_start = start; }

void RRTX::setObstacles(const std::vector<Obstacle>& obstacles) { m_obstacles = obstacles; }

void RRTX::updateObstacle(const Obstacle& updated_obstacle) {
  bool found = false;
  for (auto& obs : m_obstacles) {
    if (obs.id == updated_obstacle.id) {
      obs = updated_obstacle;
      found = true;
      break;
    }
  }
  if (!found) {
    m_obstacles.push_back(updated_obstacle);
  }
}

void RRTX::setRecalculationTime(double time_ms) { m_config.recalculation_time_ms = time_ms; }

double RRTX::distance(const Point2D& p1, const Point2D& p2) const {
  return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

Point2D RRTX::steer(const Point2D& nearest, const Point2D& sample) const {
  double dist = distance(nearest, sample);
  if (dist < m_config.step_size) {
    return sample;
  }
  Point2D steered;
  steered.x = nearest.x + m_config.step_size * (sample.x - nearest.x) / dist;
  steered.y = nearest.y + m_config.step_size * (sample.y - nearest.y) / dist;
  return steered;
}

bool RRTX::isCollisionFree(const Point2D& p1, const Point2D& p2) const {
  double len = distance(p1, p2);
  if (len == 0) return true;

  double dx = (p2.x - p1.x) / len;
  double dy = (p2.y - p1.y) / len;

  for (const auto& obs : m_obstacles) {
    double t = (obs.position.x - p1.x) * dx + (obs.position.y - p1.y) * dy;
    t = std::max(0.0, std::min(len, t));

    double closest_x = p1.x + t * dx;
    double closest_y = p1.y + t * dy;

    double dist_to_obs = std::hypot(closest_x - obs.position.x, closest_y - obs.position.y);
    if (dist_to_obs < (obs.radius + m_config.robot_radius)) {
      return false;  // Collision
    }
  }
  return true;
}

Point2D RRTX::sampleFree() const {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis_x(-m_config.field_length / 2, m_config.field_length / 2);
  std::uniform_real_distribution<> dis_y(-m_config.field_width / 2, m_config.field_width / 2);
  std::uniform_real_distribution<> dis_prob(0.0, 1.0);

  if (dis_prob(gen) < m_config.bias_to_goal) {
    return m_goal;
  }

  Point2D sample;
  sample.x = dis_x(gen);
  sample.y = dis_y(gen);
  return sample;
}

std::vector<Point2D> RRTX::plan() {
  if (isCollisionFree(m_start, m_goal)) {
    return {m_start, m_goal};
  }

  m_nodes.clear();
  m_nodes.emplace_back(m_start);

  auto start_time = std::chrono::steady_clock::now();
  int best_goal_node = -1;

  while (true) {
    auto current_time = std::chrono::steady_clock::now();
    double elapsed_ms =
        std::chrono::duration<double, std::milli>(current_time - start_time).count();
    if (elapsed_ms >= m_config.recalculation_time_ms) {
      break;
    }

    Point2D rand_pos = sampleFree();

    // Linear nearest neighbor search
    int nearest_idx = 0;
    double min_dist = 1e9;
    for (size_t i = 0; i < m_nodes.size(); ++i) {
      double dist = distance(m_nodes[i].position, rand_pos);
      if (dist < min_dist) {
        min_dist = dist;
        nearest_idx = (int)i;
      }
    }

    Point2D nearest_pos = m_nodes[nearest_idx].position;
    Point2D new_pos = steer(nearest_pos, rand_pos);

    if (!isCollisionFree(nearest_pos, new_pos)) {
      continue;
    }

    RRTXNode new_node(new_pos);
    new_node.parent_idx = nearest_idx;
    new_node.cost_from_start =
        m_nodes[nearest_idx].cost_from_start + distance(nearest_pos, new_pos);

    m_nodes.push_back(new_node);

    // Check if we can connect to the goal
    if (distance(new_pos, m_goal) < m_config.step_size && isCollisionFree(new_pos, m_goal)) {
      best_goal_node = (int)m_nodes.size() - 1;
      break;
    }
  }

  int curr = best_goal_node;
  if (curr == -1) {
    double min_d = 1e9;
    for (size_t i = 0; i < m_nodes.size(); ++i) {
      double d = distance(m_nodes[i].position, m_goal);
      if (d < min_d) {
        min_d = d;
        curr = (int)i;
      }
    }
  }

  std::vector<Point2D> path;
  while (curr != -1) {
    path.push_back(m_nodes[curr].position);
    curr = m_nodes[curr].parent_idx;
  }
  std::reverse(path.begin(), path.end());

  if (best_goal_node != -1) {
    path.push_back(m_goal);
  } else if (!path.empty() && isCollisionFree(path.back(), m_goal) &&
             distance(path.back(), m_goal) > 0.01) {
    path.push_back(m_goal);
  }

  return path;
}