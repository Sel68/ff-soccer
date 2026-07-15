#include "trajectory.h"

#include <iostream>
#include <numbers>
#include <vector>

#include "glm/vec2.hpp"

// /*
// Our top level trajectory abstract class to define movement across different
// waypoints given by the path planning algrorithm.
// */

// // TODO: WOULD IMPLEMENT LATER

// // class Trajectory {
// //  public:
// //   void Trajectory(vector<vec2> waypoints, double max_vel, double max_acc,
// //                   double initial_vel, double final_vel)
// //       : max_vel(max_vel),
// //         max_acc(max_acc),
// //         inital_vel(initial_vel),
// //         final_vel(final_vel) {
// //     vector<vec2> displacements;
// //     for (int i = 0; i < waypoints.length() - 1; i++) {
// //       displacements.push(waypoints[i + 1] - waypoints[i]);
// //     }

// //     for (int i = 0; i < displacements.length() - 1; i++) {
// //       if (displacement[i]) }
// //   }

// //  private:
// //   double inital_vel = 0;
// //   double final_vel = 0;

// //   vector<LSPBSegment> segments;
// // };

void SingleDOF::solveAll(double total_dist, double initial_vel) {
  const double eps = 1e-9;
  double pos_accum = 0;  // normalized

  int iter = 0;

  while (std::abs(total_dist) > eps || std::abs(initial_vel) > eps) {
    if (iter > 1000) throw std::runtime_error("solveAll stuck");

    double braking_dist = (initial_vel * initial_vel) / (2 * max_acc);

    Segment seg;

    if (initial_vel < 0.0) {
      seg = solveCase1(initial_vel);

    } else if (initial_vel > max_vel) {
      seg = solveCase3(initial_vel);

    } else if ((std::abs(initial_vel - max_vel) < eps) &&
               total_dist > braking_dist + eps) {
      seg = solveCase2_2(total_dist, initial_vel);

    } else if (initial_vel < max_vel && total_dist > braking_dist + eps) {
      seg = solveCase2_1(total_dist, initial_vel);
    } else {
      seg = solveCase2_3(initial_vel);
    }

    seg.pos_start = pos_accum;

    double distance_covered = distanceOf(seg);
    double seg_final_vel = velocityAtEnd(seg);

    t_total += seg.duration;
    pos_accum += distance_covered;
    total_dist -= distance_covered;
    initial_vel = seg_final_vel;

    // TODO: remove this -->
    // again renormalize the coordiantes
    // double s = sgn(total_dist);
    // total_dist *= s;
    // initial_vel *= s;

    // TODO: understand this -- THESSE ARE CRITICAL OR ELSE CODE BREAKS
    // THE REAON CODE BREAK IS ONLY BECAUSE COMPUTERS SUCK AT FLOATING POINT COMPARISON.

    // snap velocity onto max_vel if we've converged there within tolerance,
    // and clamp distance to avoid spurious sign flips from fp noise
    if (std::abs(initial_vel - max_vel) < eps) initial_vel = max_vel;
    if (std::abs(initial_vel) < eps) initial_vel = 0.0;
    total_dist = std::max(total_dist, 0.0);

    // DEBUG
    std::cout << "dist=" << total_dist << " vel=" << initial_vel
              << " dur=" << seg.duration << " covered=" << distance_covered
              << '\n';

    segments.push_back(seg);
  }
}

Segment SingleDOF::solveCase1(double initial_vel_seg) {
  double duration = -initial_vel_seg / max_acc;

  // position: would be filled by the caller -- therefore, placeholder: 0.0
  return Segment{max_acc, duration, initial_vel_seg, 0.0};
}

Segment SingleDOF::solveCase2_1(double total_dist, double initial_vel_seg) {
  // w1 is the velocity at t1 where t1 is the time when the vehicle has to start
  // decelerating in order to avoid overshooting the destination.
  double w1 =
      sqrt(total_dist * max_acc + (initial_vel_seg * initial_vel_seg) / 2);

  double tI = (max_vel - initial_vel_seg) / max_acc;
  double tII = (w1 - initial_vel_seg) / max_acc;

  double duration;

  if (tI < tII) {
    duration = tI;
  } else {
    duration = tII;
  }

  return Segment{max_acc, duration, initial_vel_seg, 0.0};
}

Segment SingleDOF::solveCase2_2(double total_dist, double initial_vel_seg) {
  double duration = total_dist / max_vel - max_vel / (2.0 * max_acc);
  return Segment{0.0, duration, initial_vel_seg, 0.0};
}

Segment SingleDOF::solveCase2_3(double initial_vel_seg) {
  double duration = initial_vel_seg / max_acc;
  return Segment{-max_acc, duration, initial_vel_seg, 0.0};
}

Segment SingleDOF::solveCase3(double intial_vel_seg) {
  double duration = (intial_vel_seg - max_vel) / max_acc;

  return Segment{-max_acc, duration, intial_vel_seg, 0.0};
}

double SingleDOF::velocityAt(double t) const {
  double t_elapsed = 0.0;
  for (const auto& seg : segments) {
    if (t <= t_elapsed + seg.duration) {
      double dt = t - t_elapsed;
      double v = seg.vel_start + seg.accel * dt;
      return dir_sign * v;
    }
    t_elapsed += seg.duration;
  }
  return 0.0;  // past the end: at rest
}

double SingleDOF::positionAt(double t) const {
  double t_elapsed = 0.0;
  for (const auto& seg : segments) {
    if (t <= t_elapsed + seg.duration) {
      double dt = t - t_elapsed;
      double p = seg.pos_start + seg.vel_start * dt + 0.5 * seg.accel * dt * dt;
      return dir_sign * p;
    }
    t_elapsed += seg.duration;
  }
  // past the end: return final segment's landing position
  if (!segments.empty()) {
    const auto& last = segments.back();
    return dir_sign * (last.pos_start + distanceOf(last));
  }
  return 0.0;
}

Synced2D::Synced2D(glm::vec2 initial_pos, glm::vec2 final_pos,
                   glm::vec2 initial_vel, double max_vel, double max_acc) {
  double alpha = std::numbers::pi_v<double> / 4;
  double alpha_min = 0;
  double alpha_max = std::numbers::pi_v<double> / 2;

  const double time_tol = 1e-6;

  for (int i = 0; i < 30; i++) {
    double ax_max = max_acc * std::cos(alpha);
    double ay_max = max_acc * std::sin(alpha);

    double vx_max = max_vel * std::cos(alpha);
    double vy_max = max_vel * std::sin(alpha);
    x = SingleDOF(initial_pos.x, final_pos.x, initial_vel.x, vx_max, ax_max);
    y = SingleDOF(initial_pos.y, final_pos.y, initial_vel.y, vy_max, ay_max);

    if (std::abs(x.totalTime() - y.totalTime()) < time_tol)
      break;
    else if (x.totalTime() > y.totalTime()) {
      alpha_max = alpha;
      alpha = (alpha + alpha_min) / 2;
    } else {
      alpha_min = alpha;
      alpha = (alpha + alpha_max) / 2;
    }
  }

  tf = std::max(x.totalTime(), y.totalTime());
}

double Synced2D::totalTime() { return tf; }
double Synced2D::velocityAtX(double t) const { return x.velocityAt(t); }
double Synced2D::velocityAtY(double t) const { return y.velocityAt(t); }
