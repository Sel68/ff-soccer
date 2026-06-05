#include <asio.hpp>
#include <iostream>

int main() {
  asio::io_context io_context;
  asio::ip::udp::socket udp_socket(io_context);

  // Should receive on 192.168.1.101:8080
  std::string host_ip_address = "192.168.1.101";
  int host_recv_port = 8080;
  asio::ip::udp::endpoint host_endpoint =
      asio::ip::udp::endpoint(asio::ip::address::from_string(host_ip_address), host_recv_port);

  udp_socket.open(host_endpoint.protocol());
  udp_socket.bind(host_endpoint);

  std::array<char, 1024> buffer;
  asio::ip::udp::endpoint sender_endpoint;

  while (true) {
    std::cout << "a\r\n";
    std::size_t bytes_received = udp_socket.receive_from(asio::buffer(buffer), sender_endpoint);

    std::cout << "Received " << bytes_received << " bytes from "
              << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << "\n";

    std::cout.write(buffer.data(), bytes_received);
    std::cout << "\n\n";
  }
}