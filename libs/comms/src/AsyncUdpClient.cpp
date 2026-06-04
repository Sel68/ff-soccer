#include "AsyncUdpClient.hpp"
#include <iostream>
#include <thread>
#include <future>

namespace comms {

bool AsyncUdpClient::request_response(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& req,
                                      std::vector<uint8_t>& resp, int timeout_ms) {
  try {
    asio::io_context ioc;
    std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(ioc);

    // bind ephemeral port
    sock->bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));

    std::shared_ptr<std::promise<std::vector<uint8_t>>> prom = std::make_shared<std::promise<std::vector<uint8_t>>>();
    auto fut = prom->get_future();

    // start receive
    sock->start_receive([prom, sock](const asio::ip::udp::endpoint& from, const std::vector<uint8_t>& data) {
      // set value only once
      try {
        prom->set_value(data);
      } catch (...) {}
    });

    // send request
    std::shared_ptr<std::promise<bool>> sent_prom = std::make_shared<std::promise<bool>>();
    auto sent_fut = sent_prom->get_future();
    sock->async_send_to(peer, req, [sent_prom](const asio::error_code& ec, std::size_t) {
      sent_prom->set_value(!ec);
    });

    // run io_context in background
    std::thread runner([&ioc]() { ioc.run(); });

    bool sent_ok = sent_fut.get();
    if (!sent_ok) {
      ioc.stop();
      runner.join();
      sock.reset();
      return false;
    }

    // wait for response
    if (fut.wait_for(std::chrono::milliseconds(timeout_ms)) == std::future_status::ready) {
      resp = fut.get();
      ioc.stop();
      runner.join();
      sock.reset();
      return true;
    }

    ioc.stop();
    runner.join();
    sock.reset();
    return false;
  } catch (std::exception& e) {
    std::cerr << "AsyncUdpClient exception: " << e.what() << std::endl;
    return false;
  }
}

}  // namespace comms
