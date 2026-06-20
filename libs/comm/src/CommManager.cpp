#include "CommManager.h"

#include <iostream>

CommManager::CommManager() : socket(io_context) {
  running = true;
  std::cout << "[CommManager::CommManager]: Start\r\n";

  // 1. Host Setup
  SetupHost();

  // 2. Base Station Setup
  SetupBaseStation();

  transmission_thread = std::thread(&CommManager::TransmissionThread, this);
  reception_thread = std::thread(&CommManager::ReceptionThread, this);
}

asio::ip::udp::endpoint CommManager::MakeEndpoint(const uint8_t ip[4], uint16_t port) {
  asio::ip::address_v4::bytes_type bytes = {ip[0], ip[1], ip[2], ip[3]};

  return asio::ip::udp::endpoint(asio::ip::address_v4(bytes), port);
}

void CommManager::SetupHost() {
  host_endpoint = std::make_unique<asio::ip::udp::endpoint>(
      MakeEndpoint(NetworkConfig::host_ip, NetworkConfig::host_port));

  socket.open(host_endpoint->protocol());

  try {
    socket.bind(*host_endpoint);

  } catch (const std::exception& e) {
    std::cout << "[CommManager::CommManager]: Exception: " << e.what() << std::endl;
    std::cout << COLOR_RED
              << "[CommManager::CommManager]: Please connect the ethernet (base station) AND "
                 "power the device"
              << COLOR_RESET << std::endl;
    exit(0);
  }
}

void CommManager::SetupBaseStation() {
  base_station_endpoint = std::make_unique<asio::ip::udp::endpoint>(
      MakeEndpoint(NetworkConfig::base_station_ip, NetworkConfig::base_station_port));
}

void CommManager::TransmissionThread() {
  int seq_id = 0;

  while (running) {
    // 1. Wait until you see ready
    {
      std::unique_lock<std::mutex> lock(transmission_mtx);
      transmission_cv.wait(lock, [&] { return received_ready && new_stored_commands; });
      received_ready = false;
      new_stored_commands = false;
      // if predicate is true -> It gets out and moves forward
      // if predicate is false -> (1) Releases the lock (2) Goes to sleep
      // On notify: (1) Wakes up (2) Checks predicate again
    }

    // std::cout << "[CommManager::TransmissionThread]: Running\r\n";

    socket.send_to(asio::buffer(stored_commands), *base_station_endpoint);
    std::cout << "[CommManager::TransmissionThread]: Sent " << stored_commands << std::endl;
  }
}

void CommManager::ReceptionThread() {
  asio::ip::udp::endpoint sender_endpoint;
  std::array<char, NetworkConfig::udp_buffer_max_size> received_data;

  while (running) {
    size_t bytes_received = socket.receive_from(asio::buffer(received_data), sender_endpoint);
    std::string msg(received_data.data(), bytes_received);

    std::cout << "[CommManager::ReceptionThread]: Received from "
              << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << " -> "
              << msg << std::endl;

    ProcessBaseStationMessage(msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void CommManager::ProcessBaseStationMessage(const std::string& msg) {
  if (msg == "ready") {
    {
      std::unique_lock<std::mutex> lock(transmission_mtx);
      received_ready = true;

      if (!new_stored_commands)
        std::cout << "[CommManager::ProcessBaseStationMessage]: WARNING!!! Received ready but NO "
                     "stored commands\r\n";
    }
    transmission_cv.notify_one();
  }
}

void CommManager::Exit() {
  running = false;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (transmission_thread.joinable()) transmission_thread.join();
  if (reception_thread.joinable()) reception_thread.join();
}

CommManager::~CommManager() { Exit(); }

void CommManager::SetRobotCommands(
    const std::array<RobotCommandMsg, SystemConstants::num_robots + 1>& robot_cmd_msg) {
  std::cout << "[CommManager::SetRobotCommands]: Setting robot commands\r\n";
  stored_commands.clear();
  for (int i = 1; i <= SystemConstants::num_robots; ++i) {
    std::string cmd_i = SerializeRobotCommandMsg(robot_cmd_msg[i]);
    uint16_t len = static_cast<uint16_t>(cmd_i.size());
    std::cout << "[CommManager::SetRobotCommands]: Robot " << i << " command size: " << len
              << std::endl;
    stored_commands.append(reinterpret_cast<const char*>(&len), sizeof(len));
    stored_commands.append(cmd_i);
  }
  {
    std::unique_lock<std::mutex> lock(transmission_mtx);
    new_stored_commands = true;
  }

  transmission_cv.notify_one();
}