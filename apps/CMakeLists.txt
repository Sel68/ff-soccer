add_executable(game_engine SimulatorApp.cpp)
target_link_libraries(game_engine PRIVATE game glfw)

add_executable(physical_engine PhysicalApp.cpp)
target_link_libraries(physical_engine PRIVATE game glfw comm_lib timing_lib)
