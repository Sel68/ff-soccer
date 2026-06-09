#ifndef RRTX_H
#define RRTX_H

#include <cmath>
#include <memory>
#include <vector>

#include "Point2D.h"
#include "Obstacle.h"
#include "kdTree.h"

// RRTX tree  node
struct RRTXNode {
  Point2D position;
  double cost_from_start;
  double lmc;  // Local Marginal Cost
  int parent_idx;
  std::vector<int> children_indices;

  RRTXNode(const Point2D& pos) : position(pos), cost_from_start(1e9), lmc(1e9), parent_idx(-1) {}
};

class RRTX {
 public:
  RRTX();
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
  double m_recalculation_time_ms;

  kdt::KDTree<Point2D> m_kd_tree;
  std::vector<RRTXNode> m_nodes;

  // config
  double step_size = 0.5;
  double search_radius = 2.0;
  double robot_radius = 0.09;
  double field_length = 12.0;
  double field_width = 9.0;

  // helper funcs
  bool isCollisionFree(const Point2D& p1, const Point2D& p2) const;
  Point2D sampleFree() const;
  Point2D steer(const Point2D& nearest, const Point2D& sample) const;
  double distance(const Point2D& p1, const Point2D& p2) const;
};
#endif