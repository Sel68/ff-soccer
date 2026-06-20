file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(uart_lib ${SOURCES})
target_include_directories(uart_lib PUBLIC include)