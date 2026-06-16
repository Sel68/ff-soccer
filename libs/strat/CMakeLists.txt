file(GLOB_RECURSE STRAT_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(strat STATIC
    ${STRAT_SOURCES}
)

target_include_directories(strat
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(strat
    PUBLIC
    glm
)
