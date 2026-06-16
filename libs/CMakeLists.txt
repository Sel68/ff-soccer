if(BUILD_ON_STM)
# Libraries only used for stm
else()
    # Libraries only used for laptop
    add_subdirectory(algo)
    add_subdirectory(comms)
    add_subdirectory(strat)
    add_subdirectory(game)
    add_subdirectory(graphics)
    add_subdirectory(handler)
endif()

# Libraries used for all platforms
add_subdirectory(external)
add_subdirectory(kin)
add_subdirectory(motion)
add_subdirectory(cfg)