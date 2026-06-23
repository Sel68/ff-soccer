file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(kin_lib ${HEADERS} ${SOURCES})

target_include_directories(kin_lib PUBLIC include)
target_link_libraries(kin_lib PUBLIC cfg_lib)

if (BUILD_ON_STM)
  target_compile_options(kin_lib PRIVATE -Os)
  target_compile_options(kin_lib PRIVATE -ffunction-sections)
  target_compile_options(kin_lib PRIVATE -fdata-sections)
  target_link_options(kin_lib PRIVATE -Wl,--gc-sections)
else()
  add_subdirectory(test)
endif()