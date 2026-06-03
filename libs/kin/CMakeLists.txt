file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(kin_lib ${HEADERS} ${SOURCES})

target_include_directories(kin_lib PUBLIC include)
target_link_libraries(kin_lib PUBLIC Eigen3::Eigen)

add_subdirectory(test)