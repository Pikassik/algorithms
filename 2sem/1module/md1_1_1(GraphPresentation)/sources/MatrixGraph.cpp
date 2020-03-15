#include "../headers/MatrixGraph.h"

MatrixGraph::MatrixGraph(int size)
    : matrix_(size, std::vector<int>(size, 0)) {}

MatrixGraph::MatrixGraph(const IGraph *graph)
    : matrix_(graph->VerticesCount(),
             std::vector<int>(graph->VerticesCount(), 0)) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> next;

    graph->GetNextVertices(i, next);
    for (auto current: next) {
      ++matrix_[i][current];
    }
  }
}

void MatrixGraph::AddEdge(int from, int to) {
  ++matrix_[from][to];
}

int MatrixGraph::VerticesCount() const {
  return static_cast<int>(matrix_.size());
}

void MatrixGraph::GetNextVertices(int vertex,
                                  std::vector<int> &vertices) const {
  vertices.clear();
  for (int i = 0; i < matrix_.size(); ++i) {
    for (int j = 0; j < matrix_[vertex][i]; ++j) {
      vertices.push_back(i);
    }
  }
}

void MatrixGraph::GetPrevVertices(int vertex,
                                  std::vector<int> &vertices) const {
  vertices.clear();
  for (int i = 0; i < matrix_.size(); ++i) {
    for (int j = 0; j < matrix_[i][vertex]; ++j) {
      vertices.push_back(i);
    }
  }
}