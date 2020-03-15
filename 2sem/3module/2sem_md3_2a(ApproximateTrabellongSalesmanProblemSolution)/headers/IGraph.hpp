#pragma once
#include <vector>

template<class Weight>
struct IGraph {

  struct Edge {
    Edge(int next, Weight weight): next(next), weight(weight) {}
    int next;
    Weight weight;
  };

  virtual ~IGraph() = default;

  // Добавление ребра от from к to.
  virtual void AddEdge(int from, int to, Weight weight) = 0;

  virtual int VerticesCount() const = 0;

  virtual std::vector<Edge> GetNextVertices(int vertex) const = 0;
};
