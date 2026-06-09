add_executable(sandbox SimulatorApp.cpp)

target_link_libraries(sandbox
    PRIVATE
    game
    glfw
)
