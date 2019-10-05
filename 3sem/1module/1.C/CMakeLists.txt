cmake_minimum_required(VERSION 3.14)
project(1_C)

set(CMAKE_CXX_STANDARD 17)
set(${CMAKE_CXX_FLAGS} "Ofast")
add_executable(1_C main.cpp)