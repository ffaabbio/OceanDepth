cmake_minimum_required(VERSION 3.10)
project(OceanDepths C)

set(CMAKE_C_STANDARD 99)

# Include header files
include_directories(include)

# Add all source files
add_executable(OceanDepths
        src/main.c
        src/player.c
        src/creature.c
        src/fight.c
        src/inventory.c
)