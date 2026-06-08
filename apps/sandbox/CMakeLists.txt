file(GLOB_RECURSE SANDBOX_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
)

add_executable(sandbox
    ${SANDBOX_SOURCES}
)

target_link_libraries(sandbox
    PRIVATE
        game
        glfw
)
