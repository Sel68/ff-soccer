file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(timing_lib ${SOURCES})
target_include_directories(timing_lib PUBLIC include)