#include <asio.hpp>

#include "Game.h"
#include "HostComm.h"
#include "OmniKinematics.h"
#include "Timing.h"
#include "Transmitter.h"

using Clock = std::chrono::steady_clock;

RobotCommands PrepareRobotCommands(Game& soccer) {
  RobotCommands robot_cmds;
  int robot_id = 0;
  for (GameObject* p : soccer.GetTeam1Players()) {
    robot_id++;
    robot_cmds[robot_id].id = robot_id;
    robot_cmds[robot_id].vx = p->Velocity.x;
    robot_cmds[robot_id].vy = p->Velocity.y;
    robot_cmds[robot_id].w = 243.1223;
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

    // soccer.ProcessInput(dt);
    // soccer.Update(dt);

    RobotCommands robot_cmds = PrepareRobotCommands(soccer);

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
  HostComm host_comm;
  Game soccer;

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