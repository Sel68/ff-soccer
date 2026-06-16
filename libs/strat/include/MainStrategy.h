#ifndef MAIN_STRATEGY_H
#define MAIN_STRATEGY_H

#include "Strategy.h"

class MainStrategy : public Strategy {
 public:
  StrategyResult update(const StrategyContext& ctx) override;
};

#endif