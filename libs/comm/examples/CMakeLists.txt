# Build example executables (named to match include/src symbols)
add_executable(AsyncUdpServer AsyncUdpServer.cpp)
target_link_libraries(AsyncUdpServer PRIVATE comm_lib asio)

add_executable(AsyncUdpClient AsyncUdpClient.cpp)
target_link_libraries(AsyncUdpClient PRIVATE comm_lib asio)

add_executable(BroadcastSender BroadcastSender.cpp)
target_link_libraries(BroadcastSender PRIVATE comm_lib asio)

add_executable(BroadcastReceiver BroadcastReceiver.cpp)
target_link_libraries(BroadcastReceiver PRIVATE comm_lib asio)

add_executable(UDPSender UDPSender.cpp)
target_link_libraries(UDPSender PRIVATE asio)
