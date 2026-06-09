file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(comm_lib ${SOURCES} ${HEADERS})
target_link_libraries(comm_lib PUBLIC asio)
target_include_directories(comm_lib PUBLIC include)

add_subdirectory(examples)
add_subdirectory(test)
