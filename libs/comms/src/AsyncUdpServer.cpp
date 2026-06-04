#include <iostream>

#include "AsyncUdpServer.hpp"

namespace comms {

AsyncUdpServer::AsyncUdpServer(asio::io_context& ioc, const asio::ip::udp::endpoint& local)
    : ioc_(ioc), local_(local), sock_(std::make_shared<UdpSocket>(ioc)) {}

AsyncUdpServer::~AsyncUdpServer() {}

void AsyncUdpServer::start() {
  sock_->bind(local_);
  sock_->start_receive([self = shared_from_this()](const asio::ip::udp::endpoint& peer,
                                                   const std::vector<uint8_t>& data) {
    self->on_receive(peer, data);
  });
}

void AsyncUdpServer::on_receive(const asio::ip::udp::endpoint& peer,
                                const std::vector<uint8_t>& data) {
  // Echo the data back to sender
  sock_->async_send_to(peer, data, [peer](const asio::error_code& ec, std::size_t len) {
    if (ec)
      std::cerr << "Echo send error to " << peer.address().to_string() << ":" << peer.port()
                << " - " << ec.message() << "\n";
  });
}

}  // namespace comms
