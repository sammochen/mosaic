cmake_minimum_required(VERSION 3.10)

project(ImageProcessing)

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ImageProcessing main.cpp)

# Download a local copy of stb: https://github.com/nothings/stb
set(STB_ROOT "stb/") # Path to stb
target_include_directories(ImageProcessing PUBLIC "${STB_ROOT}")
