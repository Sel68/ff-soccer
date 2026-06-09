#include "SystemCoordinates.h"

std::pair<double, double> screenToRRTX(const std::pair<double, double>& pos) {
  return {(pos.first / SystemConstants::SCREEN_WIDTH) * 12.0 - 6.0,
          (pos.second / SystemConstants::SCREEN_HEIGHT) * 9.0 - 4.5};
}

std::pair<double, double> rrtxToScreen(const std::pair<double, double>& p) {
  return {((p.first + 6.0) / 12.0 * SystemConstants::SCREEN_WIDTH),
          ((p.second + 4.5) / 9.0 * SystemConstants::SCREEN_HEIGHT)};
}

std::pair<double, double> screenToRRTX(double x, double y) { return screenToRRTX({x, y}); }

std::pair<double, double> rrtxToScreen(double x, double y) { return rrtxToScreen({x, y}); }