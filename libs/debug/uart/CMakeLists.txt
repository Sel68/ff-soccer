file(GLOB_RECURSE SOURCES "src/*.cpp")

add_compile_definitions(
  -DUSE_SMALL_BUFFER=${USE_SMALL_BUFFER}
)

add_library(uart_lib ${SOURCES})
target_include_directories(uart_lib PUBLIC include)