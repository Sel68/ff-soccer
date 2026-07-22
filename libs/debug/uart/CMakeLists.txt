file(GLOB_RECURSE SOURCES "src/*.cpp")

if(NOT DEFINED USE_SMALL_BUFFER)
    set(USE_SMALL_BUFFER 0)
endif()

add_compile_definitions(
  USE_SMALL_BUFFER=${USE_SMALL_BUFFER}
)

add_library(uart_lib ${SOURCES})
target_include_directories(uart_lib PUBLIC include)