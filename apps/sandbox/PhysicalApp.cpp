#include <iostream>

#include "Game.h"

bool running = true;

enum class ErrorFlags { LOW_BATTERY, MOTORS_FAILED, GYRO_FAILED };

struct Robot {
  double current_pose;
  double goal;
  double battery;
  double v;
  double a;
  ErrorFlags error_flags;
};

struct SystemInfo {
  std::vector<Robot> robots;
  double t;
};

std::optional<double> RunCameraStep() {
  if (camera.wait_for(2ms)) {
    return camera.data;
  } else
    return std::nullopt;
}

SystemInfo GetSystemInfoFromBaseStation() { return udp_base_station.GetCurrentData(); }

void SendPlansToBaseStation() { udp_base_station.SetNewData(current_info); }

SystemInfo RunHostStep() {
  // 1. Get current info
  SystemInfo current_info = GetSystemInfoFromBaseStation();

  // 2. Plan
  SystemInfo new_system_info = soccer.RunGameStep(current_info);

  // 3. Communicate
  SendPlansToBaseStation(new_system_info);

  // 4. Get Camera Data
  std::optional<double> new_camera_data = RunCameraStep();
  if (new_camera_data) {
    soccer.UpdateUsingCamera(new_camera_data);
  }
}

int main() {
  Game soccer;

  Host host;

  while (running) {
    host.RunHostStep();
    std::this_thread::sleep_for(10);
  }

  return EXIT_SUCCESS;
}

/*
  Timing

  Robots sending data to base station:
    60ms = 6 * 10
  Base station sending data to host:
    10ms
  Planning:
    10ms
  Host sending data to base station:
    10ms
  Base station sending data to robots:
    60ms = 6 * 10

  100ms
*/