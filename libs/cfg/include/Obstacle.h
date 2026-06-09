#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <utility>
#include "Point2D.h"

struct Obstacle {
  int id;  // Unique identifier=
  Point2D position;
  double radius;
};

#endif  // OBSTACLE_H