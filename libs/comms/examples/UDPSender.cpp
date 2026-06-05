#include <asio.hpp>
#include <iostream>
#include <string>

std::string format_message(const std::string& msg_type, const std::string& serialized_data) {
  return msg_type + ":" + std::to_string(serialized_data.length()) + ":" + serialized_data + "\n";
}

int main(int argc, char* argv[]) {
  asio::io_context io_context;
  asio::ip::udp::socket udp_socket(io_context);

  udp_socket.open(asio::ip::udp::v4());

  std::string target_ip_address = "192.168.1.101";  // default ip
  if (argc > 1) {
    target_ip_address = argv[1];
  }

  int target_port = 8080;  // default port
  if (argc > 2) {
    try {
      target_port = std::stoi(argv[2]);
    } catch (const std::exception& e) {
      std::cerr << "Invalid port provided, falling back to " << target_port << "\n";
    }
  }

  asio::ip::udp::endpoint target_endpoint =
      asio::ip::udp::endpoint(asio::ip::address::from_string(target_ip_address), target_port);

  std::string msg_type = "EXEC";

  // Option 1: Sending an array of commands for N (default = 6) robots
  std::string serialized_data =
      "["
      "{\"id\": 0, \"cmd\": \"move\", \"vx\": 1.0, \"vy\": 0.0}, "
      "{\"id\": 1, \"cmd\": \"stop\", \"vx\": 0.0, \"vy\": 0.0}, "
      "{\"id\": 2, \"cmd\": \"move\", \"vx\": -1.0, \"vy\": 0.5}, "
      "{\"id\": 3, \"cmd\": \"stop\", \"vx\": 0.0, \"vy\": 0.0}, "
      "{\"id\": 4, \"cmd\": \"move\", \"vx\": 0.5, \"vy\": -0.5}, "
      "{\"id\": 5, \"cmd\": \"stop\", \"vx\": 0.0, \"vy\": 0.0}"
      "]";

  std::string message = format_message(msg_type, serialized_data);

  std::cout << "Sending message to " << target_ip_address << ":" << target_port << "\n";
  std::cout << "Payload: \n" << message << "\n";

  udp_socket.send_to(asio::buffer(message), target_endpoint);

  return 0;
}
