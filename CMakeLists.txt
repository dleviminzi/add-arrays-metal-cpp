cmake_minimum_required(VERSION 3.23)
project(00)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(metal-cmake)

add_executable(00 main.cpp MetalAdder.cpp MetalAdder.h)
target_link_libraries(00 METAL_CPP)
