# Make the uart lib first
add_subdirectory(uart)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(debug_lib ${HEADERS} ${SOURCES})
target_include_directories(debug_lib PUBLIC include)

if(BUILD_ON_STM)
  target_link_libraries(debug_lib PUBLIC uart_lib)
  target_compile_options(debug_lib PRIVATE -Os)
  target_compile_options(debug_lib PRIVATE -ffunction-sections)
endif()
