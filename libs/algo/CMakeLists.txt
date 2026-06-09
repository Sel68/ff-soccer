file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(algo_lib ${HEADERS} ${SOURCES})

target_include_directories(algo_lib PUBLIC include)

add_subdirectory(test)
