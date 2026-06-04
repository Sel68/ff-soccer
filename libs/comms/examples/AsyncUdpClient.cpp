#include <asio.hpp>
#include <iostream>
#include <vector>
#include "AsyncUdpClient.hpp"

int main() {
  try {
    comms::AsyncUdpClient client;

    std::vector<uint8_t> req({'H', 'e', 'l', 'l', 'o'});
    std::vector<uint8_t> resp;
    asio::ip::udp::endpoint server_ep(asio::ip::make_address("127.0.0.1"), 9000);

    bool ok = client.request_response(server_ep, req, resp, 1000);
    if (ok) {
      std::string s(resp.begin(), resp.end());
      std::cout << "Received reply: " << s << std::endl;
    } else {
      std::cerr << "No reply received" << std::endl;
    }
  } catch (std::exception& e) {
    std::cerr << "Client error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
