cmake_minimum_required(VERSION 3.12)
project(2sem_md3_2a)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(2sem_md3_2a main.cpp headers/IGraph.hpp headers/ListGraph.hpp sources/DSU.cpp headers/DSU.h headers/GraphGenerator.h sources/GraphGenerator.cpp headers/ApproximateTravellingSalesmanProblemSolution.h sources/ApproximateTravellingSalesmanProblemSolution.cpp headers/FindMST.h sources/FindMST.cpp headers/AccurateTravellingSalesmanProblemSolution.h sources/AccurateTravellingSalesmanProblemSolution.cpp)