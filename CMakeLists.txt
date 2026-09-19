cmake_minimum_required(VERSION 3.22)
project(DHM-02 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Apply strict warnings ONLY to SLAVIC code to prevent third-party library errors
if(MSVC)
    target_compile_options(DHM-02 PRIVATE /W4)
else()
    target_compile_options(DHM-02 PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

# --- Jolt Physics Setup (Auto-Fetch) ---
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
set(TARGET_SAMPLES OFF CACHE BOOL "" FORCE)
set(TARGET_VIEWER OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(PORTABLE_COMPILER_FLAGS ON CACHE BOOL "" FORCE)
set(USE_WERROR OFF CACHE BOOL "" FORCE) # Disable strict warnings in Jolt

FetchContent_Declare(
    jolt
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG        v5.0.0
    SOURCE_SUBDIR  Build  # Important: Jolt's CMakeLists is inside its Build folder
)
FetchContent_MakeAvailable(jolt)
FetchContent_GetProperties(jolt)

# --- JSON Setup (Auto-Fetch) ---
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# --- GLFW Setup (Auto-Fetch) ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# --- Linking & Includes ---
target_link_libraries(DHM-02 PRIVATE Vulkan::Vulkan Jolt nlohmann_json::nlohmann_json glfw)

target_include_directories(DHM-02 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    ${jolt_SOURCE_DIR} # Make <Jolt/Jolt.h> visible to your code
)