file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(algo_lib ${HEADERS} ${SOURCES})
target_include_directories(algo_lib PUBLIC include)
target_link_libraries(algo_lib PRIVATE cfg_lib)

add_subdirectory(test)
add_subdirectory(bench)
