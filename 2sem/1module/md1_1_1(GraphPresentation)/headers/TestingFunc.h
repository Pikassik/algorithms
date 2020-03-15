#pragma once
#include <fstream>
#include <vector>
#include "Graphs.h"

void TestingFunc();

void TestingMethods(std::ifstream &test_file, int test_num);

// добавляет ребра в граф
void TestingEdges(std::ifstream &test_file, IGraph *graph, int edges_count);

// сравнивает выдаваемые множества входящих и исходящих ребер с ожидаемыми
void TestingPrevNext(const std::vector<std::vector<int>> &prev_vert_expected,
                     const std::vector<std::vector<int>> &next_vert_expected,
                     const IGraph *graph,
                     int vertices_count);

// генерирует случайный граф
void RandomTestGenerating();