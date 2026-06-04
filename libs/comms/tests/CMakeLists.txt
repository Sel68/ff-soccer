# Build test executable
add_executable(test_udp test_udp.cpp)
target_link_libraries(test_udp PRIVATE comms gtest gtest_main)

# Register test
enable_testing()
add_test(NAME udp_test COMMAND test_udp)
