file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(handler_lib ${HEADERS} ${SOURCES})

target_include_directories(handler_lib PUBLIC include)

target_link_libraries(handler_lib PUBLIC kin_lib comm_lib)

# add_subdirectory(examples)

# TODO:
# add_subdirectory(test)