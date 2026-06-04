file(GLOB libs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(lib ${libs})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${lib}/CMakeLists.txt" AND NOT "${lib}" STREQUAL "gtest")
    add_subdirectory(${lib})
  endif()
endforeach()