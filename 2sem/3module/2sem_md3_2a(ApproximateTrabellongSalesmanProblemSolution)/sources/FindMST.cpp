#include <headers/FindMST.h>
#include <headers/ListGraph.hpp>

std::shared_ptr<IGraph<double>> FindMST(const std::shared_ptr<IGraph<double>> graph) {
  DSU components(graph->VerticesCount());

  // пары вида <weight, <first, second> >, их сортировка по весу
  std::set<std::pair<int, std::pair<int, int>>> edges;
  for (int current = 0; current < graph->VerticesCount(); ++current) {
    for (auto next: graph->GetNextVertices(current)) {
      edges.emplace(next.weight, std::make_pair(current, next.next));
    }
  }

  // алгоритм Крускала
  std::shared_ptr<IGraph<double>> minimum_spanning_tree = std::make_shared<ListGraph<double>>(graph->VerticesCount());
  for (auto edge: edges) {
    if (components.Get(edge.second.first) !=
                                          components.Get(edge.second.second)) {
      components.Union(edge.second.first, edge.second.second);
      minimum_spanning_tree->AddEdge(edge.second.first, edge.second.second,
          edge.first);
      minimum_spanning_tree->AddEdge(edge.second.second, edge.second.first,
          edge.first);
    }
  }

  return minimum_spanning_tree;
}

