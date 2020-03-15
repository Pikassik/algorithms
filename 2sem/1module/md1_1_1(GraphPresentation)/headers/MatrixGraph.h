#pragma once
#include "IGraph.h"

class MatrixGraph : public IGraph {
 public:
  MatrixGraph() = default;

  explicit MatrixGraph(int size);

  explicit MatrixGraph(const IGraph *graph);

  ~MatrixGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void GetNextVertices(int vertex, std::vector<int>& vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int>& vertices) const override;
 private:

  // матрица смежности. в [i][j] хранится
  // количество ребер из вершины i в j
  std::vector<std::vector<int>> matrix_;
};