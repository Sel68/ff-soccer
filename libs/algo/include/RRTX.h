#ifndef RRTX_H
#define RRTX_H

#include <vector>
#include <cmath>

#include "Point2D.h"
#include "Obstacle.h"

// RRT node
struct RRTXNode {
  Point2D position;
  double cost_from_start;
  int parent_idx;

  RRTXNode(const Point2D& pos) : position(pos), cost_from_start(0.0), parent_idx(-1) {}
};

// hyperparam + other
class RRTXConfig {
 public:
  double recalculation_time_ms = 10.0;
  double bias_to_goal = 0.1;
  double step_size = 0.2;
  double search_radius = 2.0;
  double robot_radius = 0.09;
  double field_length = 12.0;
  double field_width = 9.0;
};

class RRTX {
 public:
  RRTX(const RRTXConfig& config = RRTXConfig());
  ~RRTX();

  std::vector<Point2D> PlanningStep(std::pair<double, double> start,
                                    std::pair<double, double> goal,
                                    std::vector<Obstacle> obstacles);

 private:
  void setGoal(const Point2D& goal);
  void setStart(const Point2D& start);
  void setObstacles(const std::vector<Obstacle>& obstacles);
  void updateObstacle(const Obstacle& obstacle);
  void setRecalculationTime(double time_ms);
  std::vector<Point2D> plan();

  Point2D m_start;
  Point2D m_goal;
  std::vector<Obstacle> m_obstacles;
  RRTXConfig m_config;

  std::vector<RRTXNode> m_nodes;

  // helper funcs
  bool isCollisionFree(const Point2D& p1, const Point2D& p2) const;
  Point2D sampleFree() const;
  Point2D steer(const Point2D& nearest, const Point2D& sample) const;
  double distance(const Point2D& p1, const Point2D& p2) const;
};
#endif