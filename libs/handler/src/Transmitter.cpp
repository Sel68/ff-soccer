#include "Transmitter.h"

Transmitter::Transmitter(asio::io_context& ioc, const std::string& ip, unsigned short port)
    : socket_(ioc), peer_endpoint_(asio::ip::make_address(ip), port) {
  socket_.Bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
}

void Transmitter::transmit(int robot_id, double px, double py, double ptheta,
                           const ChassisVelocity& vel,
                           const std::array<double, SystemConstants::num_drive_motors>& wheels) {
  RobotTelemetryPacket packet;
  packet.robot_id = robot_id;
  packet.pos_x = px;
  packet.pos_y = py;
  packet.pos_theta = ptheta;
  packet.vel_x = vel.vx;
  packet.vel_y = vel.vy;
  packet.vel_theta = vel.vtheta;

  for (int i = 0; i < SystemConstants::num_drive_motors; ++i) {
    packet.wheel_speeds[i] = wheels[i];
  }

  std::vector<uint8_t> data(sizeof(RobotTelemetryPacket));
  std::memcpy(data.data(), &packet, sizeof(RobotTelemetryPacket));

  socket_.async_send_to(peer_endpoint_, data);
}