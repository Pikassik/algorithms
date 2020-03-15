#pragma once
#include "IGraph.h"
#include <list>

class ArcGraph : public IGraph {
 public:
  ArcGraph() = default;

  explicit ArcGraph(int size);

  explicit ArcGraph(const IGraph *graph);

  ~ArcGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  // количество вершин
  int size_ = 0;

  // список ребер
  std::list<std::pair<int, int>> edges_;
};