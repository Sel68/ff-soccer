#include "UdpSocket.hpp"

#include <iostream>

namespace comms {

UdpSocket::UdpSocket(asio::io_context& ioc) : ioc_(ioc), socket_(ioc) {}

UdpSocket::~UdpSocket() {
  asio::error_code ec;
  socket_.close(ec);
}

void UdpSocket::bind(const asio::ip::udp::endpoint& ep) {
  socket_.open(ep.protocol());
  socket_.bind(ep);
}

void UdpSocket::start_receive(UdpHandler handler) {
  user_handler_ = std::move(handler);
  socket_.async_receive_from(
      asio::buffer(recv_buf_), remote_ep_, [this](const asio::error_code& ec, std::size_t bytes) {
        if (!ec) {
          user_handler_(remote_ep_, {recv_buf_.begin(), recv_buf_.begin() + bytes});
          start_receive(user_handler_);
        }
      });
}

void UdpSocket::async_send_to(const asio::ip::udp::endpoint& peer,
                              const std::vector<uint8_t>& data,
                              std::function<void(const asio::error_code&, std::size_t)> on_sent) {
  // static int seq = 0;
  // seq += 1;
  // std::string new_data = "hefyyy" + std::to_string(seq);
  // socket_.send_to(asio::buffer(new_data), peer);
  // std::cout << "sent data: " << new_data << std::endl;
  // left for debug^

  socket_.async_send_to(asio::buffer(data), peer,
                        [on_sent](const asio::error_code& ec, std::size_t len) {
                          if (on_sent) on_sent(ec, len);
                        });
}

void UdpSocket::set_broadcast(bool enable) {
  if (!socket_.is_open()) socket_.open(asio::ip::udp::v4());
  socket_.set_option(asio::socket_base::broadcast(enable));
}

}  // namespace comms
