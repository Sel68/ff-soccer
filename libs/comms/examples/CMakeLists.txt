# Build example executables (named to match include/src symbols)
add_executable(AsyncUdpServer AsyncUdpServer.cpp)
target_link_libraries(AsyncUdpServer PRIVATE comms)

add_executable(AsyncUdpClient AsyncUdpClient.cpp)
target_link_libraries(AsyncUdpClient PRIVATE comms)

add_executable(BroadcastSender BroadcastSender.cpp)
target_link_libraries(BroadcastSender PRIVATE comms)

add_executable(BroadcastReceiver BroadcastReceiver.cpp)
target_link_libraries(BroadcastReceiver PRIVATE comms)
