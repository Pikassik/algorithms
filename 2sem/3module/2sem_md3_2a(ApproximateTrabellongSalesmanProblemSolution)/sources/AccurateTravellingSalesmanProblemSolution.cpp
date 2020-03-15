#include <headers/AccurateTravellingSalesmanProblemSolution.h>
#include <algorithm>

double AccurateTravellingSalesmanProblemSolution(const std::shared_ptr<IGraph<double>> graph) {
  std::vector<std::vector<double>> distances_matrix(graph->VerticesCount(), std::vector<
      double>(graph->VerticesCount()));
  for (int current = 0; current < graph->VerticesCount(); ++current) {
    for (auto next: graph->GetNextVertices(current)) {
      distances_matrix[current][next.next] = next.weight;
    }
  }

  double minimum_path_length = (1.0 / 0.0); // inf

  std::vector<int> permutation;

  for (int i = 0; i < graph->VerticesCount(); ++i) {
    permutation.push_back(i);
  }

  while (true) {
    double path_length = 0;
    for (int i = 0; i < permutation.size(); ++i) {
      path_length += distances_matrix[permutation[i]][permutation[(i + 1) % permutation.size()]];
    }
    if (path_length < minimum_path_length) {
      minimum_path_length = path_length;
    }

    if (!std::next_permutation(permutation.begin(), permutation.end())) {
      break;
    }
  }

  return minimum_path_length;
}
