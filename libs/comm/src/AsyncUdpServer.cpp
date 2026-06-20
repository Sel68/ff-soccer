#include "AsyncUdpServer.hpp"

namespace comms {

AsyncUdpServer::AsyncUdpServer(asio::io_context& ioc, const asio::ip::udp::endpoint& local)
    : ioc_(ioc), local_(local), sock_(std::make_shared<UdpSocket>(ioc)) {}

AsyncUdpServer::~AsyncUdpServer() = default;

void AsyncUdpServer::start() {
  sock_->Bind(local_);
  // weak shared ptr
  sock_->start_receive([weak_self = std::weak_ptr<AsyncUdpServer>(shared_from_this())](
                           const auto& peer, const auto& data) {
    if (auto self = weak_self.lock()) {
      self->on_receive(peer, data);
    }
  });
}

void AsyncUdpServer::on_receive(const asio::ip::udp::endpoint& peer,
                                const std::vector<uint8_t>& data) {
  sock_->async_send_to(peer, data);
}

}  // namespace comms
