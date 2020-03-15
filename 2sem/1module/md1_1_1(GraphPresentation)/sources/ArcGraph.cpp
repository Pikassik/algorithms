#include "../headers/ArcGraph.h"

ArcGraph::ArcGraph(int size)
    : size_(size) {}

ArcGraph::ArcGraph(const IGraph *graph)
    : size_(graph->VerticesCount()) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> next;

    // копирование исходящих ребер
    graph->GetNextVertices(i, next);
    for (auto current: next) {
      edges_.emplace_back(i, current);
    }
  }
}

void ArcGraph::AddEdge(int from, int to) {
  edges_.emplace_back(from, to);
}

int ArcGraph::VerticesCount() const {
  return size_;
}

void ArcGraph::GetNextVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: edges_) {
    if (current.first == vertex) {
      vertices.push_back(current.second);
    }
  }
}

void ArcGraph::GetPrevVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: edges_) {
    if (current.second == vertex) {
      vertices.push_back(current.first);
    }
  }
}
