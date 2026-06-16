#ifndef STRATEGY_H
#define STRATEGY_H

#include <utility>

struct StrategyContext {
  std::pair<double, double> robot_pos;
  double robot_rotation;
  std::pair<double, double> ball_pos;
  bool has_ball;
  std::pair<double, double> goal_pos;
};

struct StrategyResult {
  std::pair<double, double> target_pos;
  bool kick;
};

class Strategy {
 public:
  virtual ~Strategy() = default;
  virtual StrategyResult update(const StrategyContext& ctx) = 0;
};

#endif
