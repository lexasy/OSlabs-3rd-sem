cmake_minimum_required(VERSION 3.10)
project(lab02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-pthread)
add_executable(lab src/generator.cpp src/bitonic_sort_parallel.cpp)