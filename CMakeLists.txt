cmake_minimum_required(VERSION 3.22)
project(robocup-pakistan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

if(BUILD_ON_STM)
  add_subdirectory(libs)
else()
  enable_testing()
  add_subdirectory(libs)
  add_subdirectory(apps)
endif()
