cmake_minimum_required(VERSION 3.10)
project(taskA)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("..")

set(CMAKE_CXX_STANDARD 14)

add_executable(taskA main.cpp
               ../StringFunctionsConversions/StringFunctionsConversions.h
               ../StringFunctionsConversions/StringFunctionsConversions.cpp)
