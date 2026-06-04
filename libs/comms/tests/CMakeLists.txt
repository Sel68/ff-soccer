# Build test executable
add_executable(udp_test TestUDP.cpp)
target_link_libraries(udp_test PRIVATE comms gtest gtest_main)

# Register test
enable_testing()
add_test(NAME udp_test COMMAND udp_test)
