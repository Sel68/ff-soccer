#pragma once
// Async UDP echo server API
#include <asio.hpp>
#include <functional>
#include <memory>
#include <vector>
#include "UdpSocket.hpp"

namespace comms {

class AsyncUdpServer : public std::enable_shared_from_this<AsyncUdpServer> {
 public:
  // Create server bound to local endpoint. Handler called for each incoming datagram.
  AsyncUdpServer(asio::io_context& ioc, const asio::ip::udp::endpoint& local);
  ~AsyncUdpServer();

  void start();

 private:
  void on_receive(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& data);

  asio::io_context& ioc_;
  asio::ip::udp::endpoint local_;
  std::shared_ptr<UdpSocket> sock_;
};

}  // namespace comms
