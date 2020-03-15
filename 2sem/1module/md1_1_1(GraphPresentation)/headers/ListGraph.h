#pragma once
#include "IGraph.h"
#include <list>

class ListGraph : public IGraph {
 public:
  ListGraph() = default;

  explicit ListGraph(int size);

  explicit ListGraph(const IGraph *graph);

  ~ListGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::list<int>> buffer_next_;

  //массив списков входящих ребер
  std::vector<std::list<int>> buffer_prev_;
};