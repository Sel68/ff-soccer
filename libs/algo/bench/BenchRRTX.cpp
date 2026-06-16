#include "RRTX.h"
#include <chrono>
#include <iostream>
#include <vector>

int main() {
  std::cout << "Starting RRTX Benchmark..." << std::endl;

  RRTXConfig config;
  config.recalculation_time_ms = 10.0;
  config.step_size = 0.2;

  RRTX planner(config);

  int num_iterations = 100;

  // Benchmark 1: No obstacles, short distance
  std::pair<double, double> start1 = {0.0, 0.0};
  std::pair<double, double> goal1 = {2.0, 2.0};
  std::vector<Obstacle> no_obstacles;

  auto t1_start = std::chrono::high_resolution_clock::now();
  int paths_found = 0;
  for (int i = 0; i < num_iterations; ++i) {
    auto path = planner.PlanningStep(start1, goal1, no_obstacles);
    if (path.size() >= 2) paths_found++;
  }
  auto t1_end = std::chrono::high_resolution_clock::now();
  double t1_duration = std::chrono::duration<double, std::milli>(t1_end - t1_start).count();

  std::cout << "Benchmark 1 (No Obstacles, Short Dist):" << std::endl;
  std::cout << "  Iterations: " << num_iterations << std::endl;
  std::cout << "  Paths Found: " << paths_found << std::endl;
  std::cout << "  Total Time: " << t1_duration << " ms" << std::endl;
  std::cout << "  Avg Time per Plan: " << t1_duration / num_iterations << " ms" << std::endl;

  // Benchmark 2: Obstacles, longer distance
  std::pair<double, double> start2 = {-4.0, -3.0};
  std::pair<double, double> goal2 = {4.0, 3.0};
  std::vector<Obstacle> obstacles;
  // create a wall
  for (double y = -2.0; y <= 2.0; y += 0.5) {
    obstacles.push_back({(int)(y * 10), {0.0, y}, 0.2});
  }

  auto t2_start = std::chrono::high_resolution_clock::now();
  paths_found = 0;
  for (int i = 0; i < num_iterations; ++i) {
    auto path = planner.PlanningStep(start2, goal2, obstacles);
    if (path.size() > 0 &&
        std::hypot(path.back().x - goal2.first, path.back().y - goal2.second) < 0.5) {
      paths_found++;
    }
  }
  auto t2_end = std::chrono::high_resolution_clock::now();
  double t2_duration = std::chrono::duration<double, std::milli>(t2_end - t2_start).count();

  std::cout << "\nBenchmark 2 (Wall of Obstacles, Long Dist):" << std::endl;
  std::cout << "  Iterations: " << num_iterations << std::endl;
  std::cout << "  Total Time: " << t2_duration << " ms" << std::endl;
  std::cout << "  Avg Time per Plan: " << t2_duration / num_iterations << " ms" << std::endl;

  // Benchmark 3: 11 moving obstacles (simulating other players on field)
  std::pair<double, double> start3 = {-5.0, -3.0};
  std::pair<double, double> goal3 = {5.0, 3.0};

  std::vector<Obstacle> dynamic_obstacles;
  for (int i = 0; i < 11; ++i) {
    // distribute them randomly between start and goal
    double x = -3.0 + (i * 0.5);
    double y = -3.0 + (i * 0.6);
    dynamic_obstacles.push_back({i, {x, y}, 0.2});
  }

  auto t3_start = std::chrono::high_resolution_clock::now();
  paths_found = 0;

  for (int i = 0; i < num_iterations; ++i) {
    // Move obstacles each iteration to simulate movement
    for (auto& obs : dynamic_obstacles) {
      obs.position.x += 0.01;
      obs.position.y += ((i % 2 == 0) ? 0.05 : -0.05);
    }

    auto path = planner.PlanningStep(start3, goal3, dynamic_obstacles);
    if (path.size() > 0 &&
        std::hypot(path.back().x - goal3.first, path.back().y - goal3.second) < 0.5) {
      paths_found++;
    }
  }
  auto t3_end = std::chrono::high_resolution_clock::now();
  double t3_duration = std::chrono::duration<double, std::milli>(t3_end - t3_start).count();

  std::cout << "\nBenchmark 3 (11 Moving Obstacles):" << std::endl;
  std::cout << "  Iterations: " << num_iterations << std::endl;
  std::cout << "  Paths Found: " << paths_found << std::endl;
  std::cout << "  Total Time: " << t3_duration << " ms" << std::endl;
  std::cout << "  Avg Time per Plan: " << t3_duration / num_iterations << " ms" << std::endl;

  return 0;
}
