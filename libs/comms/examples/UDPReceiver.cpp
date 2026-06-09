#include <asio.hpp>
#include <iostream>
#include <string>

// Standard: MSG_TYPE:PAYLOAD_LENGTH:SERIALIZED_DATA\n

// parser
void parse_message(const std::string& message) {
  size_t first_colon = message.find(':');
  if (first_colon == std::string::npos) {
    std::cerr << "Invalid format: missing first colon\n";
    return;
  }

  std::string msg_type = message.substr(0, first_colon);

  size_t second_colon = message.find(':', first_colon + 1);
  if (second_colon == std::string::npos) {
    std::cerr << "Invalid format: missing second colon\n";
    return;
  }

  std::string payload_length_str = message.substr(first_colon + 1, second_colon - first_colon - 1);
  int payload_length = 0;
  try {
    payload_length = std::stoi(payload_length_str);
  } catch (const std::exception& e) {
    std::cerr << "Invalid payload length: " << payload_length_str << "\n";
    return;
  }

  std::string serialized_data = message.substr(second_colon + 1);
  if (!serialized_data.empty() && serialized_data.back() == '\n') {
    serialized_data.pop_back();
  }

  if (serialized_data.length() != static_cast<size_t>(payload_length)) {
    std::cerr << "Warning: Payload length mismatch. Expected " << payload_length << ", got "
              << serialized_data.length() << "\n";
  }

  std::cout << "Parsed Message:\n"
            << "  MSG_TYPE: " << msg_type << "\n"
            << "  PAYLOAD_LENGTH: " << payload_length << "\n"
            << "  SERIALIZED_DATA: " << serialized_data << "\n";
}

int main(int argc, char* argv[]) {
  asio::io_context io_context;
  asio::ip::udp::socket udp_socket(io_context);

  std::string host_ip_address = "192.168.1.101";  // default ip
  if (argc > 1) {
    host_ip_address = argv[1];
  }

  int host_recv_port = 8080;  // default port
  if (argc > 2) {
    try {
      host_recv_port = std::stoi(argv[2]);
    } catch (const std::exception& e) {
      std::cerr << "Invalid port provided, falling back to " << host_recv_port << "\n";
    }
  }

  asio::ip::udp::endpoint host_endpoint(asio::ip::address::from_string(host_ip_address),
                                        host_recv_port);

  // generated error handling
  try {
    udp_socket.open(host_endpoint.protocol());
    udp_socket.bind(host_endpoint);
  } catch (const std::exception& e) {
    std::cerr << "Failed to bind to " << host_ip_address << ":" << host_recv_port << "\n"
              << "Error: " << e.what() << "\n"
              << "Make sure " << host_ip_address
              << " is the actual IP address of THIS machine (the receiver).\n"
              << "If you are unsure, you can change the code back to bind to 0.0.0.0 (all "
                 "interfaces).\n";
    return 1;
  }

  std::array<char, 1024> buffer;
  asio::ip::udp::endpoint sender_endpoint;

  std::cout << "UDP Receiver listening on " << host_ip_address << ":" << host_recv_port << "\n";

  while (true) {
    std::size_t bytes_received = udp_socket.receive_from(asio::buffer(buffer), sender_endpoint);

    std::cout << "Received " << bytes_received << " bytes from "
              << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << "\n";

    std::string msg(buffer.data(), bytes_received);
    parse_message(msg);
    std::cout << "\n";
  }
}