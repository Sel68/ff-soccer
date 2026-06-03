add_executable(kin_test OmniKinematics.cpp)
target_link_libraries(kin_test PRIVATE kin_lib gtest_main)
add_test(NAME kin_test COMMAND kin_test)
