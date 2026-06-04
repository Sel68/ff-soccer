#include "UdpSocket.hpp"
#include <iostream>

namespace comms {

UdpSocket::UdpSocket(asio::io_context& ioc) : ioc_(ioc), socket_(ioc) {}

UdpSocket::~UdpSocket() { asio::error_code ec; socket_.close(ec); }

void UdpSocket::bind(const asio::ip::udp::endpoint& ep) {
  asio::error_code ec;
  socket_.open(ep.protocol(), ec);
  if (ec) { std::cerr << "Socket open error: " << ec.message() << "\n"; return; }
  socket_.set_option(asio::ip::udp::socket::reuse_address(true), ec);
  if (ec) { std::cerr << "Socket reuse_address error: " << ec.message() << "\n"; }
  socket_.bind(ep, ec);
  if (ec) { std::cerr << "Socket bind error: " << ec.message() << "\n"; }
}

void UdpSocket::start_receive(UdpHandler handler) {
  user_handler_ = std::move(handler);
  socket_.async_receive_from(asio::buffer(recv_buf_), remote_ep_,
                             [this](const asio::error_code& ec, std::size_t bytes) {
                               if (!ec && bytes > 0) {
                                 std::vector<uint8_t> data(bytes);
                                 std::copy_n(recv_buf_.data(), bytes, data.begin());
                                 if (user_handler_) user_handler_(remote_ep_, data);
                               } else if (ec) {
                                 std::cerr << "UdpSocket receive error: " << ec.message() << "\n";
                               }
                               // continue receiving if socket still open
                               if (socket_.is_open()) start_receive(user_handler_);
                             });
}

void UdpSocket::async_send_to(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& data,
                              std::function<void(const asio::error_code&, std::size_t)> on_sent) {
  socket_.async_send_to(asio::buffer(data), peer, [on_sent](const asio::error_code& ec, std::size_t len) {
    if (on_sent) on_sent(ec, len);
  });
}

void UdpSocket::set_broadcast(bool enable) {
  asio::error_code ec;
  // ensure socket is open before setting option
  if (!socket_.is_open()) {
    socket_.open(asio::ip::udp::v4(), ec);
    if (ec) {
      std::cerr << "Socket open (for broadcast) error: " << ec.message() << "\n";
      return;
    }
  }
  socket_.set_option(asio::socket_base::broadcast(enable), ec);
  if (ec) std::cerr << "set_broadcast error: " << ec.message() << "\n";
}

}  // namespace comms
