#ifndef SYSTEM_COORDINATES_H
#define SYSTEM_COORDINATES_H

#include "SystemConstants.h"
#include "Point2D.h"

std::pair<double, double> screenToRRTX(const std::pair<double, double>& pos);
std::pair<double, double> rrtxToScreen(const std::pair<double, double>& p);
std::pair<double, double> rrtxToScreen(double x, double y);
std::pair<double, double> screenToRRTX(double x, double y);

#endif  // SYSTEM_COORDINATES_H