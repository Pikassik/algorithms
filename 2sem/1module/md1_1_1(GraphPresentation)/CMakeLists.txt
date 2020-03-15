cmake_minimum_required(VERSION 3.10)
project(2sem_md1_1_1GraphPresentation)

set(CMAKE_CXX_STANDARD 14)

add_executable(2sem_md1_1_1GraphPresentation main.cpp headers/IGraph.h headers/ListGraph.h headers/MatrixGraph.h headers/SetGraph.h headers/ArcGraph.h sources/ListGraph.cpp sources/MatrixGraph.cpp sources/SetGraph.cpp sources/ArcGraph.cpp headers/Graphs.h headers/TestingFunc.h sources/TestingFunc.cpp)
