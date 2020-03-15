#include <headers/GraphGenerator.h>
#include <headers/ListGraph.hpp>
#include <random>
#include <cmath>

std::shared_ptr<IGraph<double>> GraphGenerator(int size) {
  std::vector<std::pair<double, double>> vertices;
  std::random_device random_dev;
  std::mt19937 generator(random_dev());
  std::normal_distribution<double> random_vertex_generator;
  for (int i = 0; i < size; ++i) {
    vertices.emplace_back(random_vertex_generator(generator),
                                          random_vertex_generator(generator));
  }

  std::shared_ptr<IGraph<double>> graph =
                                      std::make_shared<ListGraph<double>>(size);
  for (int from = 0; from < size; ++from) {
    for (int to = from + 1; to < size; ++to) {
      double distance = std::sqrt((vertices[from].first - vertices[to].first) *
          (vertices[from].first - vertices[to].first) +
          (vertices[from].second - vertices[to].second) *
          (vertices[from].second - vertices[to].second));
      graph->AddEdge(from, to, distance);
      graph->AddEdge(to, from, distance);
    }
  }

  return graph;
}

