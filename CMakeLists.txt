# ==============================================================================
# SETUP
# ==============================================================================

cmake_minimum_required(VERSION 3.25)
project(morphogenesis_sandbox LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ==============================================================================
# LIBRARIES
# ==============================================================================

# Dependency versions
set(RAYLIB_TAG  6.0)
set(IMGUI_TAG   v1.92.7)
set(RLIMGUI_TAG Raylib_6_0)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # Skip raylib examples
FetchContent_Declare(raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        ${RAYLIB_TAG}
  GIT_SHALLOW    TRUE)

FetchContent_Declare(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        ${IMGUI_TAG}
  GIT_SHALLOW    TRUE)

FetchContent_Declare(rlimgui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG        ${RLIMGUI_TAG}
  GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(raylib imgui rlimgui)

# ImGui and rlImGui ship no CMakeLists, wrap them in a small library
add_library(imgui_rl STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${rlimgui_SOURCE_DIR}/rlImGui.cpp)

target_include_directories(imgui_rl PUBLIC
  ${imgui_SOURCE_DIR}
  ${rlimgui_SOURCE_DIR})

target_link_libraries(imgui_rl PUBLIC raylib)

# ==============================================================================
# APPLICATION
# ==============================================================================

add_executable(${PROJECT_NAME}
  src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib imgui_rl)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
