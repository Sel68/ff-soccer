#include "AsyncUdpClient.hpp"
#include <iostream>

namespace comms {

bool AsyncUdpClient::request_response(const asio::ip::udp::endpoint& peer,
                                      const std::vector<uint8_t>& req, std::vector<uint8_t>& resp,
                                      int timeout_ms) {
  try {
    asio::io_context ioc;
    asio::ip::udp::socket sock(ioc, asio::ip::udp::v4());

    if (peer.address().is_loopback()) {
      sock.bind(asio::ip::udp::endpoint(peer.address(), 0));
    }

    asio::error_code ec;
    sock.send_to(asio::buffer(req), peer, 0, ec);
    if (ec) return false;

    bool received = false;
    std::vector<uint8_t> buf(1400);
    asio::ip::udp::endpoint sender;

    sock.async_receive_from(asio::buffer(buf), sender,
                            [&](const asio::error_code& error, std::size_t bytes) {
                              if (!error && bytes > 0) {
                                resp.assign(buf.begin(), buf.begin() + bytes);
                                received = true;
                              }
                            });

    ioc.run_for(std::chrono::milliseconds(timeout_ms));
    return received;
  } catch (const std::exception& e) {
    std::cerr << "AsyncUdpClient error: " << e.what() << "\n";
    return false;
  }
}

}  // namespace comms
