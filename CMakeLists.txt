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

# Collect all source files (using absolute paths for safety)
file(GLOB_RECURSE DHM_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(DHM-02 ${DHM_SOURCES})

# --- Jolt Physics Setup ---
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(PORTABLE_COMPILER_FLAGS ON CACHE BOOL "" FORCE)
add_subdirectory(external/jolt)

# --- JSON Setup ---
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# --- Linking & Includes ---
target_link_libraries(DHM-02 PRIVATE Vulkan::Vulkan Jolt nlohmann_json::nlohmann_json)

target_include_directories(DHM-02 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/jolt>
)
