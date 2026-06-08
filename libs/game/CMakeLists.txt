file(GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(game STATIC
    ${GAME_SOURCES}
)

target_include_directories(game
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game
    PUBLIC
        glad
        glm
        graphics
        glfw
)

target_compile_definitions(game
    PRIVATE
        SHADER_DIR="${CMAKE_SOURCE_DIR}/libs/graphics/include/"
        TEXTURE_DIR="${CMAKE_SOURCE_DIR}/libs/graphics/texture/"
        LEVEL_DIR="${CMAKE_SOURCE_DIR}/libs/game/levels/"
)
