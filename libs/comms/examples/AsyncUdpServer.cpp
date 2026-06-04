#include <asio.hpp>
#include <iostream>
#include <memory>
#include "UdpSocket.hpp"
#include "AsyncUdpServer.hpp"

int main() {
  try {
    asio::io_context ioc;
    auto server = std::make_shared<comms::AsyncUdpServer>(
        ioc, asio::ip::udp::endpoint(asio::ip::udp::v4(), 9000));
    server->start();

    std::cout << "UDP echo server listening on port 9000" << std::endl;
    ioc.run();
  } catch (std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
