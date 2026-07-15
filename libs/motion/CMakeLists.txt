file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(motion_lib ${HEADERS} ${SOURCES})

target_include_directories(motion_lib PUBLIC include)
target_link_libraries(motion_lib PUBLIC cfg_lib glm)

if(BUILD_ON_STM)
  target_compile_options(motion_lib PRIVATE -Os)
  target_compile_options(motion_lib PRIVATE -ffunction-sections)
  target_compile_options(motion_lib PRIVATE -fdata-sections)
  target_link_options(motion_lib PRIVATE -Wl,--gc-sections)
endif()
