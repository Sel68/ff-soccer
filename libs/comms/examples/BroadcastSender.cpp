#include <asio.hpp>
#include <iostream>
#include <memory>
#include <chrono>
#include "UdpSocket.hpp"

// BroadcastSender: periodically broadcasts a short message on the local network
int main() {
  try {
    asio::io_context ioc;
    comms::UdpSocket sock(ioc);

    // bind to any address on an ephemeral port so OS picks interface
    sock.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    sock.set_broadcast(true);

    asio::ip::udp::endpoint broadcast_ep(asio::ip::address_v4::broadcast(), 9005);

    int count = 0;
    const int max_count = 10;
    asio::steady_timer timer(ioc);

    std::function<void()> send_loop;
    send_loop = [&]() {
      if (count >= max_count) {
        // stop the io_context after we've sent the required number
        ioc.stop();
        return;
      }
      std::string msg = "broadcast:" + std::to_string(count++);
      std::vector<uint8_t> data(msg.begin(), msg.end());
      sock.async_send_to(broadcast_ep, data, [](const asio::error_code& ec, std::size_t) {
        if (ec) std::cerr << "Broadcast send error: " << ec.message() << "\n";
      });
      timer.expires_after(std::chrono::milliseconds(500));
      timer.async_wait([&](const asio::error_code&){ send_loop(); });
    };

    send_loop();
    std::cout << "BroadcastSender running, sending to 255.255.255.255:9005" << std::endl;
    ioc.run();
  } catch (std::exception& e) {
    std::cerr << "BroadcastSender error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
