#pragma once
// Async UDP client with request-response and timeout
#include <asio.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "UdpSocket.h"

namespace comms {

class AsyncUdpClient : public std::enable_shared_from_this<AsyncUdpClient> {
 public:
  AsyncUdpClient() = default;

  // Send a request to peer and wait for a single response (returns true on success)
  // timeout_ms: milliseconds to wait for reply
  bool request_response(const asio::ip::udp::endpoint& peer, const std::vector<uint8_t>& req,
                        std::vector<uint8_t>& resp, int timeout_ms = 1000);
};

}  // namespace comms
