cmake_minimum_required(VERSION 3.16)

project(SLAVIC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES
    core/*.cpp
    simulation/*.cpp
    persistence/*.cpp
    utils/*.cpp
    main.cpp
)

add_executable(SLAVIC ${SOURCES})