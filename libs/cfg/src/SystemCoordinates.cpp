#include "SystemCoordinates.h"

std::pair<double, double> screenToRRTX(const std::pair<double, double> &pos) {
  return {(pos.first / SystemConstants::screen_width) * 12.0 - 6.0,
          (pos.second / SystemConstants::screen_height) * 9.0 - 4.5};
}

std::pair<double, double> rrtxToScreen(const std::pair<double, double> &p) {
  return {((p.first + 6.0) / 12.0 * SystemConstants::screen_width),
          ((p.second + 4.5) / 9.0 * SystemConstants::screen_height)};
}

std::pair<double, double> screenToRRTX(double x, double y) {
  return screenToRRTX({x, y});
}

std::pair<double, double> rrtxToScreen(double x, double y) {
  return rrtxToScreen({x, y});
}