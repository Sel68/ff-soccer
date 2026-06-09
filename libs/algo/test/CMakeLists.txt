add_executable(algo_test TestRRTX.cpp)
target_link_libraries(algo_test PRIVATE algo_lib gtest_main cfg_lib)
add_test(NAME algo_test COMMAND algo_test)
