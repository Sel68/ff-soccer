file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(cfg_lib ${HEADERS} ${SOURCES})
target_include_directories(cfg_lib PUBLIC include)