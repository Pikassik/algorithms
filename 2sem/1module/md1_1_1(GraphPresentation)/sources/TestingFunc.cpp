#include "../headers/TestingFunc.h"
#include "../headers/ArcGraph.h"
#include "../headers/Graphs.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <set>
#include <ratio>

void TestingFunc() {
  RandomTestGenerating();
  std::ifstream test0("../tests/test0");
  std::ifstream test1("../tests/test1");
  std::ifstream test2("../tests/test2");
  std::ifstream test3("../tests/test3");
  std::ifstream test4("../tests/test4");
  std::ifstream test5("../tests/test5");
  std::ifstream test6("../tests/test6");
  std::ifstream test7("../tests/test7");
  std::ifstream test8("../tests/test8");
  std::ifstream test9("../tests/test9");
  std::ifstream test10("../tests/test10");
  std::ifstream testrandom("../tests/testrandom");

  TestingMethods(test0, 0);
  TestingMethods(test1, 1);
  TestingMethods(test2, 2);
  TestingMethods(test3, 3);
  TestingMethods(test4, 4);
  TestingMethods(test5, 5);
  TestingMethods(test6, 6);
  TestingMethods(test7, 7);
  TestingMethods(test8, 8);
  TestingMethods(test9, 9);
  TestingMethods(test10, 10);
  TestingMethods(testrandom, 11);
}

void TestingMethods(std::ifstream &test_file, int test_num) {
  int vertices_count;
  int edges_count;
  test_file >> vertices_count;
  test_file >> edges_count;

  IGraph *graph0 = new ArcGraph(vertices_count);
  TestingEdges(test_file, graph0, edges_count);

  test_file.get(); // костыль

  std::vector<std::vector<int>>
      prev_vert_expected(vertices_count, std::vector<int>());
  std::vector<std::vector<int>>
      next_vert_expected(vertices_count, std::vector<int>());

  std::string string_prev;
  std::string string_next;

  for (int i = 0; i < vertices_count; ++i) {
    getline(test_file, string_prev);
    getline(test_file, string_next);

    std::basic_istringstream<char> str_stream_prev(string_prev);
    std::basic_istringstream<char> str_stream_next(string_next);

    // считывание из файла ожидаемых значений
    while (!str_stream_prev.eof() && string_prev != "") {
      int vertice;
      str_stream_prev >> vertice;
      prev_vert_expected[i].push_back(vertice);
    }
    std::sort(prev_vert_expected[i].begin(), prev_vert_expected[i].end());

    while (!str_stream_next.eof() && string_next != "") {
      int vertice;
      str_stream_next >> vertice;
      next_vert_expected[i].push_back(vertice);
    }
    std::sort(next_vert_expected[i].begin(), next_vert_expected[i].end());
  }

  TestingPrevNext(prev_vert_expected, next_vert_expected, graph0,
                  vertices_count);

  IGraph *graph1 = new MatrixGraph(graph0);
  TestingPrevNext(prev_vert_expected, next_vert_expected, graph1,
                  vertices_count);

  IGraph *graph2 = new ListGraph(graph1);
  TestingPrevNext(prev_vert_expected, next_vert_expected, graph2,
                  vertices_count);

  IGraph *graph3 = new SetGraph(graph2);
  TestingPrevNext(prev_vert_expected, next_vert_expected, graph3,
                  vertices_count);

  IGraph *graph4 = new ArcGraph(graph3);
  TestingPrevNext(prev_vert_expected, next_vert_expected, graph4,
                  vertices_count);

  std::cout << "Test " << test_num << " completed succesfully" << std::endl;
}

void TestingEdges(std::ifstream &test_file, IGraph *graph, int edges_count) {
  int from, to;
  for (int i = 0; i < edges_count; ++i) {
    test_file >> from >> to;
    graph->AddEdge(from, to);
  }
}
void TestingPrevNext(const std::vector<std::vector<int>> &prev_vert_expected,
                     const std::vector<std::vector<int>> &next_vert_expected,
                     const IGraph *graph,
                     int vertices_count) {
  assert(graph->VerticesCount() == vertices_count);

  for (int i = 0; i < graph->VerticesCount(); ++i,
      assert(graph->VerticesCount() == vertices_count)) {

    std::vector<int> prev_vert_returned;
    graph->GetPrevVertices(i, prev_vert_returned);
    std::sort(prev_vert_returned.begin(), prev_vert_returned.end());
    assert(prev_vert_expected[i] == prev_vert_returned);

    std::vector<int> next_vert_returned;
    graph->GetNextVertices(i, next_vert_returned);
    std::sort(next_vert_returned.begin(), next_vert_returned.end());
    assert(next_vert_expected[i] == next_vert_returned);
  }
}

void RandomTestGenerating() {
  std::ofstream testrandom("../tests/testrandom");

  int vertices_count = 10000;
  int edges_count = 10000;

  testrandom << vertices_count << " ";
  testrandom << edges_count << std::endl;

  // генерация случайных ребер
  std::multiset<std::pair<int, int>> edges;
  for (int i = 0; i < edges_count; ++i) {
    edges.insert(std::make_pair(rand() % vertices_count,
                             rand() % vertices_count));
  }

  // вывод в файл ребер
  for (auto current: edges) {
    testrandom << current.first << " " << current.second << std::endl;
  }

  // создание графа и генерация псевдо-верных ответов
  IGraph *graph = new SetGraph(vertices_count);
  for (auto current: edges) {
    graph->AddEdge(current.first, current.second);
  }
  for (int i = 0; i < vertices_count; ++i) {

    std::vector<int> prev_vertices;
    graph->GetPrevVertices(i, prev_vertices);
    for (auto current: prev_vertices) {
      testrandom << current << " ";
    }

    if (prev_vertices.size() != 0) {
      testrandom.seekp(-1, std::ios_base::cur);
    }

    testrandom << std::endl;

    std::vector<int> next_vertices;
    graph->GetNextVertices(i, next_vertices);
    for (auto current: next_vertices) {
      testrandom << current << " ";
    }

    if (next_vertices.size() != 0) {
      testrandom.seekp(-1, std::ios_base::cur);
    }

    testrandom << std::endl;
  }
}