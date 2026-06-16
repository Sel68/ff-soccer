#include <cmath>

#include "MainStrategy.h"

StrategyResult MainStrategy::update(const StrategyContext& ctx) {
  StrategyResult result;
  result.kick = false;

  if (ctx.has_ball) {
    double dist_to_goal = std::hypot(ctx.robot_pos.first - ctx.goal_pos.first,
                                     ctx.robot_pos.second - ctx.goal_pos.second);

    double angle_to_goal = std::atan2(ctx.goal_pos.second - ctx.robot_pos.second,
                                      ctx.goal_pos.first - ctx.robot_pos.first);
    double norm_rot = std::fmod(ctx.robot_rotation, 360.0);
    if (norm_rot < 0) norm_rot += 360.0;
    double robot_angle_rad = norm_rot * M_PI / 180.0;

    // take care of atan convention
    double angle_diff = std::fmod(robot_angle_rad - angle_to_goal + M_PI, 2 * M_PI);
    if (angle_diff < 0) angle_diff += 2 * M_PI;
    angle_diff -= M_PI;

    result.target_pos = ctx.goal_pos;
    // ~10 degree threshold, 0.2 in radians
    if (dist_to_goal < 250.0 && std::abs(angle_diff) < 0.2) {  // kick threshold
      result.kick = true;
    }
  } else {
    // Find ball, target little behind the ball relative to the goal
    result.target_pos = ctx.ball_pos; // Default target is the ball

    double dx = ctx.ball_pos.first - ctx.goal_pos.first;
    double dy = ctx.ball_pos.second - ctx.goal_pos.second;
    double dist = std::hypot(dx, dy);

    double offset = 50.0;  // Distance behind the ball to target
    if (dist > 0.001) {
      double back_x = ctx.ball_pos.first + (dx / dist) * offset;
      double back_y = ctx.ball_pos.second + (dy / dist) * offset;

      double dist_to_back =
          std::hypot(ctx.robot_pos.first - back_x, ctx.robot_pos.second - back_y);
      if (dist_to_back >= 60.0) {
        // We aren't close to the setup point yet, move to the setup point instead of diving
        result.target_pos.first = back_x;
        result.target_pos.second = back_y;
      }
    }
  }

  return result;
}
