#ifndef SYS_POINT_2D_H
#define SYS_POINT_2D_H

#include <utility>

struct Point2D {
  static const int DIM = 2;
  double x;
  double y;

  Point2D() {}

  Point2D(double x_, double y_) : x(x_), y(y_) {}

  Point2D(std::pair<double, double> point) {
    x = point.first;
    y = point.second;
  }

  // handy for dim access
  double operator[](int idx) const { return idx == 0 ? x : y; }
};

#endif  // SYS_POINT_2D_H