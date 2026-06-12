if (BUILD_ON_STM)
# Libraries only used for stm
else()
    # Libraries only used for laptop
    add_subdirectory(asio)
    add_subdirectory(glad)
    add_subdirectory(glfw)
    add_subdirectory(glm)
    add_subdirectory(gtest)
    add_subdirectory(stb_image)
endif()

# Libraries used for all platforms
add_subdirectory(eigen)