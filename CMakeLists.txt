cmake_minimum_required(VERSION 3.22)
project(robocup-pk)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Build on stm? ${BUILD_ON_STM}")

if(BUILD_ON_STM)
  add_definitions(-DBUILD_ON_STM)  
  add_subdirectory(libs)
else()
  enable_testing()
  add_subdirectory(libs)
  add_subdirectory(apps)
endif()
