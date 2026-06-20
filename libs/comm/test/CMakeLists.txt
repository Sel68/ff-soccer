add_executable(udp_test TestUDP.cpp)
target_link_libraries(udp_test PRIVATE comm_lib gtest gtest_main asio)
add_test(NAME udp_test COMMAND udp_test)
