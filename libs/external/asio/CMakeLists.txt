cmake_minimum_required(VERSION 3.14)
project(asio-proj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(asio INTERFACE)

target_include_directories(asio INTERFACE include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
# UNIX (mac/linux) only
find_package(Threads REQUIRED)
target_link_libraries(asio INTERFACE Threads::Threads)