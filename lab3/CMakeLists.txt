cmake_minimum_required(VERSION 3.10)
project(lab03)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_compile_options(-pthread)
add_executable(child1 ${SOURCE_DIR}/child1.c ${SOURCE_DIR}/mstring.c)
add_executable(child2 ${SOURCE_DIR}/child2.c ${SOURCE_DIR}/mstring.c)
add_executable(parent ${SOURCE_DIR}/parent.c ${SOURCE_DIR}/mstring.c)