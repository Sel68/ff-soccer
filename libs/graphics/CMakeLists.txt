file(GLOB_RECURSE GRAPHICS_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(graphics STATIC
    ${GRAPHICS_SOURCES}
)

target_include_directories(graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(graphics
    PUBLIC
        glad
        glm
        stb_image
        game
)