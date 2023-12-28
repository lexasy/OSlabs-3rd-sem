cmake_minimum_required(VERSION 3.10)
project(lab5-7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-lzmq")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(calculator include/topology.hpp src/topology.cpp include/socket.hpp 
        src/socket.cpp src/calculator_node.cpp
)
target_include_directories(calculator PRIVATE include)

add_executable(controller include/topology.hpp src/topology.cpp include/socket.hpp
        src/socket.cpp src/controller_node.cpp
)


target_include_directories(controller PRIVATE include)

target_link_libraries(calculator PRIVATE zmq)

target_link_libraries(controller PRIVATE zmq)