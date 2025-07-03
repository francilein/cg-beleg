cmake_minimum_required(VERSION 3.10)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    main.cpp
    src/light.cpp
    src/raytracer.cpp
    src/renderer.cpp
    src/kdtree.cpp
    src/stb_image_write.cpp
)

# Create executable
add_executable(raytracer ${SOURCES})

# Compiler flags for optimization
target_compile_options(raytracer PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native>
    $<$<CONFIG:Debug>:-g -O0>
)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
