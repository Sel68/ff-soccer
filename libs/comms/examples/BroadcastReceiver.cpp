#include <asio.hpp>
#include <iostream>
#include <memory>
#include "UdpSocket.hpp"

// BroadcastReceiver: listens on port 9005 for broadcast messages
int main() {
  try {
    asio::io_context ioc;
    comms::UdpSocket sock(ioc);
    // bind to all interfaces on port 9005
    sock.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 9005));

    int recv_count = 0;
    const int max_recv = 10;  // stop after this many packets
    sock.start_receive([&ioc, &recv_count](const asio::ip::udp::endpoint& peer,
                                           const std::vector<uint8_t>& data) {
      std::string s(data.begin(), data.end());
      std::cout << "Received from " << peer.address().to_string() << ":" << peer.port() << " -> "
                << s << std::endl;
      if (++recv_count >= max_recv) {
        // stop the event loop after receiving enough packets
        ioc.stop();
      }
    });

    std::cout << "BroadcastReceiver listening on port 9005" << std::endl;
    ioc.run();
  } catch (std::exception& e) {
    std::cerr << "BroadcastReceiver error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
