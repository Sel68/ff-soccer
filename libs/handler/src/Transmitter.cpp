#include "Transmitter.h"

Transmitter::Transmitter(asio::io_context& ioc, const std::string& ip, unsigned short port)
    : socket_(ioc), peer_endpoint_(asio::ip::make_address(ip), port) {
  socket_.bind(asio::ip::udp::endpoint(asio::ip::address::from_string(ip), port));
}

void Transmitter::transmit(int robot_id, double px, double py, double ptheta,
                           const ChassisVelocity& vel, const Eigen::VectorXd& wheels) {
  RobotTelemetryPacket packet;
  packet.robot_id = robot_id;
  packet.pos_x = px;
  packet.pos_y = py;
  packet.pos_theta = ptheta;
  packet.vel_x = vel.vx;
  packet.vel_y = vel.vy;
  packet.vel_theta = vel.vtheta;

  int num_wheels = std::min(static_cast<int>(wheels.size()), 4);
  for (int i = 0; i < 4; ++i) {
    if (i < num_wheels) {
      packet.wheel_speeds[i] = wheels(i);
    } else {
      packet.wheel_speeds[i] = 0.0;
    }
  }

  std::vector<uint8_t> data(10);
  data[0] = 0x11;
  data[1] = 0x11;
  data[2] = 0x11;
  data[3] = 0x11;
  data[4] = 0x11;
  // std::memcpy(data.data(), &packet, sizeof(RobotTelemetryPacket));

  // host: 192.168.1.101:8080
  // 192.168.1.100:8081
  int device_udp_port = 8081;
  std::string device_ip = "192.168.1.100";
  asio::ip::address ip_addr = asio::ip::address::from_string(device_ip);
  asio::ip::udp::endpoint remote_endpoint =
      asio::ip::udp::endpoint(ip_addr, device_udp_port);

  // socket_.

  socket_.async_send_to(remote_endpoint, data);
}