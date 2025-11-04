cmake_minimum_required(VERSION 4.0)
project(OceanDepth C)

set(CMAKE_C_STANDARD 17)

if (MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)
add_executable(OceanDepth
        src/main.c
        src/creature.c
        src/player.c
        src/inventory.c
        src/fight.c
        src/save.c
        src/skills.c
)