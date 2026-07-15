#pragma once

#include <cmath>
#include <vector>
#include "glm/glm.hpp"

/*
Constant Acceleration segment of the trajectory

In NORMALIZED coordiantes, vel_start and pos_start: here start represent start
of the segment. Using the attribtues of the segment, we can determine pos(t) and
vel(t) at any t inside the segment.
*/
struct Segment {
  double accel;     // constant acc applied during the segment
  double duration;  // duration of the segment
  double vel_start;
  double pos_start;
};

enum class CaseType {
  Case1,
  Case2_1,
  Case2_2,
  Case2_3,
  Case3
};

class SingleDOF{
public:
  SingleDOF() = default;
  SingleDOF(double initial_pos, double final_pos, double initial_vel,
            double max_vel, double max_acc)
      : 
        max_vel(max_vel),
        max_acc(max_acc) {

    // --- Normalize the velocity and position
    double total_dist = final_pos - initial_pos;

    double s = sgn(total_dist);
    dir_sign = s;

    total_dist *= s;
    initial_vel *= s;
   
    solveAll(total_dist, initial_vel);
  }
  
  static double sgn(double num) {
    return num < 0.0 ? -1.0 : 1.0;
  }

  static double distanceOf(const Segment& seg) {
    // simple S = Vi t + 1/2 a t^2
    return seg.vel_start * seg.duration + 0.5 * seg.accel * seg.duration * seg.duration;
  }
  
  static double velocityAtEnd(const Segment& seg) {
    return seg.vel_start + seg.accel * seg.duration;
  }

  const std::vector<Segment>& get_segments() const { return segments; }
  double totalTime() const { return t_total; }


  Segment solveCase1(double initial_vel);
  Segment solveCase2_1(double total_dist, double initial_vel_seg);
  Segment solveCase2_2(double total_dist, double initial_vel_seg);
  Segment solveCase2_3(double initial_vel);
  Segment solveCase3(double initial_vel);
  void solveAll(double final_pos, double initial_vel);


  double velocityAt(double t) const;
  double positionAt(double t) const;

 private:
  double max_vel = 0.0; // defaults
  double max_acc = 0.0; // defaults

  double t_total = 0.0;

  double dir_sign = 1.0;
  std::vector<Segment> segments;
};


class Synced2D {
  SingleDOF x, y;
  double tf;

 public:
  Synced2D() = default;
  Synced2D(glm::vec2 initial_pos, glm::vec2 final_pos, glm::vec2 initial_vel,
           double max_vel, double max_acc);

  double totalTime();
  double velocityAtX(double t) const;
  double velocityAtY(double t) const;
};
