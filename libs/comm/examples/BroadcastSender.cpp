#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>

#include "UdpSocket.h"

// BroadcastSender: periodically broadcasts a short message on the local network
int main() {
  try {
    asio::io_context ioc;
    comms::UdpSocket sock(ioc);

    // bind to any address on an ephemeral port so OS picks interface
    sock.Bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    sock.set_broadcast(true);

    asio::ip::udp::endpoint broadcast_ep(asio::ip::address_v4::broadcast(), 9005);

    int count = 0;
    const int max_count = 10;
    asio::steady_timer retry_timer(ioc);
    asio::steady_timer delay_timer(ioc);

    std::function<void()> send_current;

    sock.start_receive([&](const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& data) {
      std::string s(data.begin(), data.end());
      if (s == "ACK:" + std::to_string(count)) {
        retry_timer.cancel();
        std::cout << "Received " << s << " from " << peer.address().to_string() << ":"
                  << peer.port() << "\n";
        count++;
        if (count >= max_count) {
          ioc.stop();
        } else {
          delay_timer.expires_after(std::chrono::milliseconds(500));
          delay_timer.async_wait([&](const asio::error_code& ec) {
            if (!ec) send_current();
          });
        }
      }
    });

    send_current = [&]() {
      if (count >= max_count) return;
      std::string msg = "broadcast:" + std::to_string(count);
      std::vector<uint8_t> data(msg.begin(), msg.end());
      sock.async_send_to(broadcast_ep, data, [](const asio::error_code& ec, std::size_t) {
        if (ec) std::cerr << "Broadcast send error: " << ec.message() << "\n";
      });
      // Set a quick timeout to resend if no ACK is received
      retry_timer.expires_after(std::chrono::milliseconds(200));
      retry_timer.async_wait([&](const asio::error_code& ec) {
        if (!ec) {
          std::cout << "Timeout, resending " << msg << "\n";
          send_current();  // retry on timeout
        }
      });
    };

    send_current();
    std::cout << "BroadcastSender running, sending to 255.255.255.255:9005" << std::endl;
    ioc.run();
  } catch (std::exception& e) {
    std::cerr << "BroadcastSender error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
