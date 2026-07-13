#include <asio.hpp>
#include <iostream>

#include "Game.h"
#include "HostComm.h"
#include "CameraComm.h"
#include "OmniKinematics.h"
#include "Timing.h"
#include "Transmitter.h"

using Clock = std::chrono::steady_clock;

RobotCommands PrepareRobotCommands(Game& soccer, const std::vector<CameraValues>& cam_vals) {
  RobotCommands robot_cmds;
  
  int robot_id = 0;
  for (GameObject* p : soccer.GetTeam1Players()) {
    
    robot_id++;
    // if (robot_id > cam_vals.size()) break;
    int cam_idx = robot_id - 1;

    if (cam_idx < cam_vals.size()) {
      robot_cmds[robot_id].id = cam_vals[cam_idx].id;
      robot_cmds[robot_id].camX = cam_vals[cam_idx].x;
      robot_cmds[robot_id].camY = cam_vals[cam_idx].y;
      robot_cmds[robot_id].camTheta = cam_vals[cam_idx].orientation;
    } else {
      robot_cmds[robot_id].id = robot_id;
      robot_cmds[robot_id].camX = 0.0f;
      robot_cmds[robot_id].camY = 0.0f;
      robot_cmds[robot_id].camTheta = 0.0f;
    }

    robot_cmds[robot_id].vx = p->velocity.x;
    robot_cmds[robot_id].vy = p->velocity.y;
    robot_cmds[robot_id].w = 243.1223;
    robot_cmds[robot_id].chargeVal = p->charge;

    std::cout << robot_cmds[robot_id].id << " " << robot_cmds[robot_id].vx << " "
              << robot_cmds[robot_id].vy << " " << robot_cmds[robot_id].w << " "
              << robot_cmds[robot_id].camX << " " << robot_cmds[robot_id].camY << " "
              << robot_cmds[robot_id].camTheta << " " << robot_cmds[robot_id].chargeVal
              << std::endl;
  }
  return robot_cmds;
}

class Host {
 public:
  Host() {
    soccer.SetCurrentAlgo(AlgoName::RRTX);
    t_last = t_current = Clock::now();
    running = true;
  }

  void LoopStep() {
    t_current = Clock::now();
    double dt = std::chrono::duration<double>(t_current - t_last).count();
    t_last = t_current;

    std::vector<CameraValues> cam_curr_values = CameraComm::GetCameraValues();

    if (!cam_curr_values.empty()) {
      soccer.ProcessInput(dt, cam_curr_values[0].x, cam_curr_values[0].y,
                          cam_curr_values[0].orientation);
    } else {
      soccer.ProcessInput(dt, 0.0f, 0.0f, 0.0f);
    }

    soccer.ProcessInput(dt, 50, 30, 20);
    soccer.Update(dt);

    RobotCommands robot_cmds = PrepareRobotCommands(soccer, cam_curr_values);

    host_comm.SetRobotCommands(robot_cmds);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 100 Hz
  }

  bool Running() { return soccer.Running() && running; }

  void Exit() {
    host_comm.Exit();
    soccer.Exit();
  }

  ~Host() { Exit(); }

 private:
  Game soccer;
  HostComm host_comm;

  // Timing
  Clock::time_point t_current, t_last;

  bool running = false;
};

bool physical_app_running = true;

void SignalHandler(int signum) {
  std::cout << "\nSIGINT received. Shutting down...\n";
  physical_app_running = 0;
}

uint32_t HostClockFunction() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

int main() {
  // 1. System Init
  // std::signal(SIGINT, SignalHandler);
  RegisterClock(HostClockFunction);

  // 2. Host
  Host host;

  while (host.Running() && physical_app_running) {
    host.LoopStep();
  }

  host.Exit();

  return 0;
}