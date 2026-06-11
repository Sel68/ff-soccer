file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(cfg_lib ${HEADERS} ${SOURCES})
target_include_directories(cfg_lib PUBLIC include)

if (BUILD_ON_STM)
  target_compile_options(cfg_lib PRIVATE -Os)
  target_compile_options(cfg_lib PRIVATE -ffunction-sections)
  target_compile_options(cfg_lib PRIVATE -fdata-sections)
  target_link_options(cfg_lib PRIVATE -Wl,--gc-sections)
else()
  # add_subdirectory(test)
endif()