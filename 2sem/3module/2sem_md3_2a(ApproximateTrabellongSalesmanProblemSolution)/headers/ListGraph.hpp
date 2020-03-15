#pragma once
#include <headers/IGraph.hpp>

template<class Weight>
class ListGraph : public IGraph<Weight> {
 public:
  ListGraph() = default;

  explicit ListGraph(int size);

  explicit ListGraph(const IGraph<Weight> *graph);

  ~ListGraph() override = default;

  void AddEdge(int from, int to, Weight weight) override;

  int VerticesCount() const override;

  std::vector<class IGraph<Weight>::Edge>
  GetNextVertices(int vertex) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<class IGraph<Weight>::Edge>> adjacency_list_;
};

template<class Weight>
ListGraph<Weight>::ListGraph(int size)
    : adjacency_list_(size) {}

template<class Weight>
ListGraph<Weight>::ListGraph(const IGraph<Weight> *graph)
    : adjacency_list_(graph->VerticesCount()) {

  for (int i = 0; i < graph->VerticesCount(); ++i) {
    adjacency_list_[i] = graph->GetNextVertices(i);
  }
}

template<class Weight>
void ListGraph<Weight>::AddEdge(int from, int to, Weight weight) {
  adjacency_list_[from].emplace_back(to, weight);
}

template<class Weight>
int ListGraph<Weight>::VerticesCount() const {
  return adjacency_list_.size();
}

template<class Weight>
std::vector<class IGraph<Weight>::Edge> ListGraph<Weight>::GetNextVertices
    (int vertex) const {
  return adjacency_list_[vertex];
}
