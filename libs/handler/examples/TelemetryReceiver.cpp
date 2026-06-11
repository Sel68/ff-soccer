#include <array>
#include <asio.hpp>
#include <iostream>

#pragma pack(push, 1)
struct RobotTelemetryPacket {
  int robot_id;
  double pos_x;
  double pos_y;
  double pos_theta;
  double vel_x;
  double vel_y;
  double vel_theta;
  double wheel_speeds[4];
};
#pragma pack(pop)

int main(int argc, char* argv[]) {
  asio::io_context io_context;
  asio::ip::udp::socket udp_socket(io_context);

  std::string host_ip_address = "127.0.0.1";
  int host_recv_port = 12345;

  if (argc > 1) host_ip_address = argv[1];
  if (argc > 2) host_recv_port = std::stoi(argv[2]);

  asio::ip::udp::endpoint host_endpoint(asio::ip::address::from_string(host_ip_address),
                                        host_recv_port);

  try {
    udp_socket.open(host_endpoint.protocol());
    udp_socket.bind(host_endpoint);
  } catch (const std::exception& e) {
    std::cerr << "Failed to bind to " << host_ip_address << ":" << host_recv_port
              << "\n" file(GLOB_RECURSE HEADERS "include/*.h")
                     file(GLOB_RECURSE SOURCES "src/*.cpp")

                         add_library(handler_lib ${HEADERS} ${SOURCES})

                             target_include_directories(handler_lib PUBLIC include)

                                 target_link_libraries(handler_lib PUBLIC kin_lib comm_lib)

                                     add_subdirectory(examples)

#TODO:
#add_subdirectory(test)
              << "Error: " << e.what() << "\n";
    return 1;
  }

  std::array<uint8_t, 1024> buffer;
  asio::ip::udp::endpoint sender_endpoint;

  std::cout << "Telemetry Receiver listening on " << host_ip_address << ":" << host_recv_port
            << "\n";
  std::cout << "Waiting for packets...\n\n";

  while (true) {
    std::size_t bytes_received = udp_socket.receive_from(asio::buffer(buffer), sender_endpoint);

    if (bytes_received == sizeof(RobotTelemetryPacket)) {
      RobotTelemetryPacket* packet = reinterpret_cast<RobotTelemetryPacket*>(buffer.data());
      std::cout << "[Robot " << packet->robot_id << "] "
                << "Pos: (" << packet->pos_x << ", " << packet->pos_y << ", " << packet->pos_theta
                << ") | "
                << "Vel: (" << packet->vel_x << ", " << packet->vel_y << ", " << packet->vel_theta
                << ") | "
                << "Wheels: [" << packet->wheel_speeds[0] << ", " << packet->wheel_speeds[1]
                << ", " << packet->wheel_speeds[2] << ", " << packet->wheel_speeds[3] << "]\n";
    } else {
      std::cerr << "Received packet of unexpected size: " << bytes_received << " bytes.\n";
    }
  }

  return 0;
}
