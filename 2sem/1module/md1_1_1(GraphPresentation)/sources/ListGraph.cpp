#include "../headers/ListGraph.h"

ListGraph::ListGraph(int size)
    : buffer_next_(size, std::list<int>())
    , buffer_prev_(size, std::list<int>()) {}

ListGraph::ListGraph(const IGraph *graph)
    : buffer_next_(graph->VerticesCount(), std::list<int>())
    , buffer_prev_(graph->VerticesCount(), std::list<int>()) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> vertices;

    // копирование исходящих ребер
    graph->GetNextVertices(i, vertices);
    for (auto current: vertices) {
      buffer_next_[i].push_back(current);
    }

    // копирование входящих ребер
    graph->GetPrevVertices(i, vertices);
    for (auto current: vertices) {
      buffer_prev_[i].push_back(current);
    }
  }
}

void ListGraph::AddEdge(int from, int to) {
  buffer_next_[from].push_back(to);
  buffer_prev_[to].push_back(from);
}

int ListGraph::VerticesCount() const {
  return static_cast<int>(buffer_next_.size());
}

void ListGraph::GetNextVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_next_[vertex]) {
    vertices.push_back(current);
  }
}

void ListGraph::GetPrevVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_prev_[vertex]) {
    vertices.push_back(current);
  }
}
