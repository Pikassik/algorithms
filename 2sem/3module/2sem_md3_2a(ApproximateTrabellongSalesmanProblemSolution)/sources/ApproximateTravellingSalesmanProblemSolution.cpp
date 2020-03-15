#include <headers/ApproximateTravellingSalesmanProblemSolution.h>
#include <headers/FindMST.h>

double ApproximateTravellingSalesmanProblemSolution(const std::shared_ptr<IGraph<double>> graph) {
  std::shared_ptr<IGraph<double>> minimum_spanning_tree = FindMST(graph);
  std::vector<int> visited(graph->VerticesCount(), 0);
  std::vector<int> stack;
  std::vector<int> path;

  stack.push_back(0);
  while (!stack.empty()) {
    int current = stack.back();
    visited[current] = true;
    path.push_back(current);
    stack.pop_back();

    for (auto next: minimum_spanning_tree->GetNextVertices(current)) {
      if (!visited[next.next]) {
        stack.push_back(next.next);
      }
    }
  }

  path.push_back(0);

  double path_length = 0;
  for (int i = 0; i < path.size() - 1; ++i) {
    for (auto next: graph->GetNextVertices(path[i])) {
      if (next.next == path[i + 1]) {
        path_length += next.weight;
        break;
      }
    }
  }

  return path_length;
}

