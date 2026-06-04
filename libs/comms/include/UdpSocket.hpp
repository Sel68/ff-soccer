#pragma once
// Lightweight UDP socket wrapper using standalone Asio
#include <asio.hpp>
#include <functional>
#include <vector>
#include <array>

namespace comms {

using UdpHandler = std::function<void(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& data)>;

class UdpSocket {
 public:
  explicit UdpSocket(asio::io_context& ioc);
  ~UdpSocket();

  // bind to local endpoint (address and port)
  void bind(const asio::ip::udp::endpoint& ep);

  // start receiving (will call handler for each datagram)
  void start_receive(UdpHandler handler);

  // async send to peer; handler called on completion with error_code
  void async_send_to(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& data,
                     std::function<void(const asio::error_code&, std::size_t)> on_sent = {});

  // enable or disable broadcast on the socket
  void set_broadcast(bool enable);

 private:
  asio::io_context& ioc_;
  asio::ip::udp::socket socket_;
  std::array<uint8_t, 1400> recv_buf_;
  asio::ip::udp::endpoint remote_ep_;
  UdpHandler user_handler_;
};

}  // namespace comms
