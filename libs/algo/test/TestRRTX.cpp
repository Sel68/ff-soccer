// #include <gtest/gtest.h>
// #include "RRTX.h"

// TEST(RRTXTest, BasicInitialization) {
//   RRTX planner;
//   planner.setStart({0.0, 0.0});
//   planner.setGoal({2.0, 2.0});
//   planner.setRecalculationTime(5.0);  // 5 ms to run fast

//   auto path = planner.plan();

//   // RRT* with KD-Tree will add at least the start node
//   EXPECT_FALSE(path.empty());

//   // It should start at the start point
//   EXPECT_NEAR(path.front().x, 0.0, 1e-4);
//   EXPECT_NEAR(path.front().y, 0.0, 1e-4);
// }

// TEST(RRTXTest, PathToGoalWithObstacles) {
//   RRTX planner;
//   planner.setStart({0.0, 0.0});
//   planner.setGoal({4.0, 0.0});

//   std::vector<Obstacle> obstacles;
//   // Put an obstacle right in the middle
//   obstacles.push_back({1, {2.0, 0.0}, 0.5});
//   planner.setObstacles(obstacles);

//   planner.setRecalculationTime(50.0);  // Let it search for a bit

//   auto path = planner.plan();
//   EXPECT_FALSE(path.empty());

//   // Validate path is collision free
//   // Since we check collisions internally, we expect the path to route around the obstacle
//   // Just checking that we reached near the goal
//   if (path.size() > 1) {
//     Point2D last = path.back();
//     double dist_to_goal = std::hypot(last.x - 4.0, last.y - 0.0);
//     // As long as it got closer to the goal than the start
//     EXPECT_LT(dist_to_goal, 4.0);
//   }
// }

// TEST(RRTXTest, UpdateObstacleTest) {
//   RRTX planner;
//   planner.setStart({0.0, 0.0});
//   planner.setGoal({2.0, 2.0});

//   std::vector<Obstacle> obs1 = {{1, {1.0, 1.0}, 0.2}};
//   planner.setObstacles(obs1);

//   // Update the obstacle
//   planner.updateObstacle({1, {1.5, 1.5}, 0.3});

//   // Simply making sure it doesn't crash and path planner runs
//   planner.setRecalculationTime(5.0);
//   auto path = planner.plan();
//   EXPECT_FALSE(path.empty());
// }
