cmake_minimum_required(VERSION 4.0)
project(OceanDepth C)

set(CMAKE_C_STANDARD 17)

include_directories(include)
add_executable(OceanDepth
        src/main.c
        include/creature.h
        src/creature.c
        include/common.h
)
