#pragma once
#include "IGraph.h"
#include <unordered_set>

class SetGraph : public IGraph {
 public:
  SetGraph() = default;

  explicit SetGraph(int size);

  explicit SetGraph(const IGraph *graph);

  ~SetGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  // массив хеш-таблиц исходящих ребер
  std::vector<std::unordered_multiset<int>> buffer_next_;

  // массив хеш-таблиц входящих ребер
  std::vector<std::unordered_multiset<int>> buffer_prev_;
};