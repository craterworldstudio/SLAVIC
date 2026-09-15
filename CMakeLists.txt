cmake_minimum_required(VERSION 3.22)
project(DHM-Core VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Global Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Collect Source Files
file(GLOB_RECURSE DHM_SOURCES 
    "src/*.cpp"
    "src/*.h"
)

# Main Executable Target
add_executable(DHM-Core ${DHM_SOURCES})

target_include_directories(DHM-Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
