#include "../headers/SetGraph.h"

SetGraph::SetGraph(int size)
    : buffer_next_(size, std::unordered_multiset<int>())
    , buffer_prev_(size, std::unordered_multiset<int>()) {}

SetGraph::SetGraph(const IGraph *graph)
    : buffer_next_(graph->VerticesCount(), std::unordered_multiset<int>())
    , buffer_prev_(graph->VerticesCount(), std::unordered_multiset<int>()) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> vertices;

    // копирование исходящих ребер
    graph->GetNextVertices(i, vertices);
    for (auto current: vertices) {
      buffer_next_[i].insert(current);
    }

    // копирование входящих ребер
    graph->GetPrevVertices(i, vertices);
    for (auto current: vertices) {
      buffer_prev_[i].insert(current);
    }
  }
}

void SetGraph::AddEdge(int from, int to) {
  buffer_next_[from].insert(to);
  buffer_prev_[to].insert(from);
}

int SetGraph::VerticesCount() const {
  return static_cast<int>(buffer_next_.size());
}

void SetGraph::GetNextVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_next_[vertex]) {
    vertices.push_back(current);
  }
}

void SetGraph::GetPrevVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_prev_[vertex]) {
    vertices.push_back(current);
  }
}

