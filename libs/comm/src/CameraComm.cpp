#include "CameraComm.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <thread>
#include <chrono>
#include <regex>

CameraComm& CameraComm::GetInstance() {
  static CameraComm instance;
  return instance;
}

CameraComm::CameraComm() : sock_fd(-1), running(true) {
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    std::cerr << "[CameraComm] Error creating socket\n";
    running = false;
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
    running = false;
    return;
  }

  std::cout << "[CameraComm] Listening for binary data on port " << LISTEN_PORT << "...\n";
  recv_thread = std::thread(&CameraComm::ReceiveLoop, this);
}

CameraComm::~CameraComm() {
  running = false;
  if (recv_thread.joinable()) {
    recv_thread.join();
  }
  if (sock_fd >= 0) {
    close(sock_fd);
  }
}

void CameraComm::ReceiveLoop() {
  while (running) {
    if (sock_fd < 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    ssize_t bytes_received = recvfrom(sock_fd, buffer.data(), BUFFER_SIZE, MSG_DONTWAIT,
                                      (struct sockaddr*)&client_addr, &client_len);

    if (bytes_received < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
      } else {
        std::cerr << "[CameraComm] Error receiving network bytes\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      continue;
    }

    std::string str(buffer.data(), bytes_received);
    
    // Regex to match: "id": <num>, "x": <num>, "y": <num>, "orientation": <num>
    std::regex re(R"(\"id\"\s*:\s*(\d+)\s*,\s*\"x\"\s*:\s*([+-]?([0-9]*[.])?[0-9]+)\s*,\s*\"y\"\s*:\s*([+-]?([0-9]*[.])?[0-9]+)\s*,\s*\"orientation\"\s*:\s*([+-]?([0-9]*[.])?[0-9]+))");
    
    auto words_begin = std::sregex_iterator(str.begin(), str.end(), re);
    auto words_end = std::sregex_iterator();

    std::vector<CameraValues> discovered_robots;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        CameraValues cv;
        cv.id = std::stoi(match[1].str());
        cv.x = std::stof(match[2].str());
        cv.y = std::stof(match[4].str());
        cv.orientation = std::stof(match[6].str());
        discovered_robots.push_back(cv);
    }

    if (!discovered_robots.empty()) {
      std::lock_guard<std::mutex> lock(values_mutex);
      latest_values = std::move(discovered_robots);
    }
  }
}

std::vector<CameraValues> CameraComm::GetCameraValues() {
  std::lock_guard<std::mutex> lock(GetInstance().values_mutex);
  return GetInstance().latest_values;
}