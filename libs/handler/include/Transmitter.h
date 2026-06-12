#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>
#include <cstring>
#include <algorithm>

#include <asio.hpp>

#include "OmniKinematics.h"
#include "UdpSocket.hpp"

// optmised byte layout
#pragma pack(push, 1)
struct RobotTelemetryPacket {
  int robot_id;
  float pos_x;
  float pos_y;
  float pos_theta;
  float vel_x;
  float vel_y;
  float vel_theta;
  float wheel_speeds[4];
};
#pragma pack(pop)

class Transmitter {
 private:
  comms::UdpSocket socket_;
  asio::ip::udp::endpoint peer_endpoint_;

 public:
  Transmitter(asio::io_context& ioc, const std::string& ip, unsigned short port);

  // didnt  make it static to allow multi transmissions
  void transmit(int robot_id, double px, double py, double ptheta, const ChassisVelocity& vel,
                const Eigen::VectorXd& wheels);
};

#endif