cmake_minimum_required(VERSION 3.22)
project(DHM-02 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
)

# Collect all source files
file(GLOB_RECURSE DHM_SOURCES 
    "src/*.cpp"
    "src/*.h"
)

add_executable(DHM-02 ${DHM_SOURCES})

target_link_libraries(DHM-02 PRIVATE Vulkan::Vulkan)

target_include_directories(DHM-02 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
