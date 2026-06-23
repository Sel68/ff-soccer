#ifndef HOST_COMM_H
#define HOST_COMM_H

#include <asio.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "EncodingDecoding.h"
#include "Logger.h"
#include "MessageInterface.h"
#include "NetworkConfig.h"
#include "SystemConstants.h"

class HostComm {
 public:
  HostComm();
  ~HostComm();
  void Exit();

  void SetRobotCommands(
      const std::array<RobotCommandMsg, SystemConstants::NUM_ROBOTS + 1>& robot_command_msg);

 private:
  void TransmissionThread();
  void ReceptionThread();
  void SetupBaseStation();
  asio::ip::udp::endpoint MakeEndpoint(const uint8_t ip[4], uint16_t port);
  void SetupHost();
  void ProcessBaseStationMessage(const std::string& msg);

  asio::io_context io_context;
  asio::ip::udp::socket socket;

  bool running = false;
  bool received_ready = false;

  std::thread transmission_thread;
  std::thread reception_thread;

  std::unique_ptr<asio::ip::udp::endpoint> host_endpoint;
  std::unique_ptr<asio::ip::udp::endpoint> base_station_endpoint;

  std::mutex transmission_mtx;
  std::condition_variable transmission_cv;

  // Current Robot Commands
  bool new_stored_commands = false;
  std::string stored_commands;
};

#endif  // HOST_COMM_H