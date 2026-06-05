#include <asio.hpp>
#include <iostream>
#include <string>

std::string format_message(const std::string& msg_type, const std::string& serialized_data) {
  return msg_type + ":" + std::to_string(serialized_data.length()) + ":" + serialized_data + "\n";
}

int main() {
  asio::io_context io_context;
  asio::ip::udp::socket udp_socket(io_context);

  udp_socket.open(asio::ip::udp::v4());

  std::string target_ip_address = "192.168.1.101";
  int target_port = 8080;
  asio::ip::udp::endpoint target_endpoint =
      asio::ip::udp::endpoint(asio::ip::address::from_string(target_ip_address), target_port);

  std::string msg_type = "EXEC";
  std::string serialized_data = "{\"command\": \"move\", \"velocity\": 1.5}";

  std::string message = format_message(msg_type, serialized_data);

  std::cout << "Sending message to " << target_ip_address << ":" << target_port << "\n";
  std::cout << message;

  udp_socket.send_to(asio::buffer(message), target_endpoint);

  return 0;
}
