#ifndef MAIN_STRATEGY_H
#define MAIN_STRATEGY_H

#include "Strategy.h"

enum class ActionState { NONE, SEEKING_SETUP, SEEKING_BALL, AIMING_AT_GOAL, KICKING };

class MainStrategy : public Strategy {
 public:
  static bool m_debug_mode;
  static void setDebugMode(bool mode) { m_debug_mode = mode; }

  StrategyResult update(const StrategyContext& ctx) override;

 private:
  ActionState last_state = ActionState::NONE;
};

#endif