add_executable(demo_msg DemoEndToEnd.cpp)
target_link_libraries(demo_msg PRIVATE msg_lib debug_lib cfg_lib)