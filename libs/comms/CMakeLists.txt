## Build instructions (from this directory):
# mkdir -p build && cd build
# cmake .. && cmake --build .

cmake_minimum_required(VERSION 3.15)
project(comms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

find_path(ASIO_INCLUDE_DIR asio.hpp)
if(NOT ASIO_INCLUDE_DIR)
  message(FATAL_ERROR "asio.hpp not found. Install standalone Asio and set ASIO_INCLUDE_DIR or update CMAKE_MODULE_PATH.")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/gtest gtest_build EXCLUDE_FROM_ALL)

add_library(comms STATIC
  src/UdpSocket.cpp
  src/AsyncUdpServer.cpp
  src/AsyncUdpClient.cpp
)

target_include_directories(comms PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ASIO_INCLUDE_DIR}
)

target_link_libraries(comms PUBLIC Threads::Threads)

enable_testing()

add_subdirectory(examples)
add_subdirectory(tests)
