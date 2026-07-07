#include "CameraComm.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

CameraComm::CameraComm() : sock_fd(-1) {
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    std::cerr << "[CameraComm] Error creating socket\n";
    return;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LISTEN_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "[CameraComm] Bind failed\n";
    close(sock_fd);
    sock_fd = -1;
    return;
  }

  std::cout << "[CameraComm] Listening for binary data on port " << LISTEN_PORT << "...\n";
}

CameraComm::~CameraComm() {
  if (sock_fd >= 0) {
    close(sock_fd);
  }
}

std::vector<CameraValues> CameraComm::GetCameraValues() {
  std::vector<CameraValues> discovered_robots;

  if (sock_fd < 0) {
    return discovered_robots;
  }

  std::vector<char> buffer(BUFFER_SIZE);
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);

  ssize_t bytes_received = recvfrom(sock_fd, buffer.data(), BUFFER_SIZE, 0,
                                    (struct sockaddr*)&client_addr, &client_len);

  if (bytes_received < 0) {
    std::cerr << "[CameraComm] Error receiving network bytes\n";
    return discovered_robots;
  }

  size_t struct_size = sizeof(CameraValues);
  if (bytes_received % struct_size != 0) {
    std::cerr << "[CameraComm] Warning: Received corrupted packet size (" << bytes_received
              << " bytes). Expected a multiple of " << struct_size << "\n";
    return discovered_robots;
  }

  size_t robot_count = bytes_received / struct_size;
  discovered_robots.resize(robot_count);

  std::memcpy(discovered_robots.data(), buffer.data(), bytes_received);

  return discovered_robots;
}