#include "RRTX.h"
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>

RRTX::RRTX() : m_recalculation_time_ms(10.0) {}

RRTX::~RRTX() {}

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

void RRTX::setRecalculationTime(double time_ms) { m_recalculation_time_ms = time_ms; }

double RRTX::distance(const Point2D& p1, const Point2D& p2) const {
  return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

Point2D RRTX::steer(const Point2D& nearest, const Point2D& sample) const {
  double dist = distance(nearest, sample);
  if (dist < step_size) {
    return sample;
  }
  Point2D steered;
  steered.x = nearest.x + step_size * (sample.x - nearest.x) / dist;
  steered.y = nearest.y + step_size * (sample.y - nearest.y) / dist;
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
    if (dist_to_obs < (obs.radius + robot_radius)) {
      return false;  // Collision
    }
  }
  return true;
}

Point2D RRTX::sampleFree() const {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis_x(-field_length / 2, field_length / 2);
  static std::uniform_real_distribution<> dis_y(-field_width / 2, field_width / 2);
  static std::uniform_real_distribution<> dis_prob(0.0, 1.0);

  if (dis_prob(gen) < 0.1) {
    return m_goal;
  }

  Point2D sample;
  sample.x = dis_x(gen);
  sample.y = dis_y(gen);
  return sample;
}

std::vector<Point2D> RRTX::plan() {
  std::vector<Point2D> path;

  if (m_nodes.empty()) {
    m_nodes.emplace_back(m_start);
    m_nodes[0].cost_from_start = 0;
    m_kd_tree.insert(m_start);
  }

  auto start_time = std::chrono::steady_clock::now();

  // Basic RRT* loop to simulate RRTX growth within time limits
  while (true) {
    auto current_time = std::chrono::steady_clock::now();
    double elapsed_ms =
        std::chrono::duration<double, std::milli>(current_time - start_time).count();
    if (elapsed_ms >= m_recalculation_time_ms) {
      break;
    }

    Point2D rand_pos = sampleFree();

    int nearest_idx = m_kd_tree.nnSearch(rand_pos);
    if (nearest_idx < 0 || nearest_idx >= (int)m_nodes.size()) continue;

    Point2D nearest_pos = m_nodes[nearest_idx].position;
    Point2D new_pos = steer(nearest_pos, rand_pos);

    if (!isCollisionFree(nearest_pos, new_pos)) {
      continue;
    }

    int new_node_idx = (int)m_nodes.size();
    RRTXNode new_node(new_pos);

    std::vector<int> near_indices = m_kd_tree.radiusSearch(new_pos, search_radius);

    // Find best parent
    int best_parent_idx = nearest_idx;
    double min_cost = m_nodes[nearest_idx].cost_from_start + distance(nearest_pos, new_pos);

    for (int near_idx : near_indices) {
      if (near_idx >= new_node_idx) continue;
      Point2D near_pos = m_nodes[near_idx].position;
      if (isCollisionFree(near_pos, new_pos)) {
        double cost = m_nodes[near_idx].cost_from_start + distance(near_pos, new_pos);
        if (cost < min_cost) {
          min_cost = cost;
          best_parent_idx = near_idx;
        }
      }
    }

    new_node.parent_idx = best_parent_idx;
    new_node.cost_from_start = min_cost;
    m_nodes.push_back(new_node);
    m_kd_tree.insert(new_pos);
    m_nodes[best_parent_idx].children_indices.push_back(new_node_idx);

    // Rewire near nodes
    for (int near_idx : near_indices) {
      if (near_idx >= new_node_idx) continue;
      Point2D near_pos = m_nodes[near_idx].position;
      double new_cost = min_cost + distance(new_pos, near_pos);
      if (new_cost < m_nodes[near_idx].cost_from_start && isCollisionFree(new_pos, near_pos)) {
        m_nodes[near_idx].parent_idx = new_node_idx;
        m_nodes[near_idx].cost_from_start = new_cost;
      }
    }
  }

  int closest_to_goal = -1;
  double min_dist_to_goal = 1e9;
  for (size_t i = 0; i < m_nodes.size(); ++i) {
    double dist = distance(m_nodes[i].position, m_goal);
    if (dist < min_dist_to_goal) {
      min_dist_to_goal = dist;
      closest_to_goal = (int)i;
    }
  }

  if (closest_to_goal != -1) {
    int curr = closest_to_goal;
    while (curr != -1) {
      path.push_back(m_nodes[curr].position);
      curr = m_nodes[curr].parent_idx;
    }
    std::reverse(path.begin(), path.end());
    if (min_dist_to_goal > 0.01 && isCollisionFree(path.back(), m_goal)) {
      path.push_back(m_goal);
    }
  }

  return path;
}
