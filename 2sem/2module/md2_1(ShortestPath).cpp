// A. Города
// Требуется отыскать самый короткий маршрут между городами.
// Из города может выходить дорога, которая возвращается в этот же город.
// Требуемое время работы O((N + M)log N),
// где N – количество городов, M – известных дорог между ними.
// N ≤ 10000, M ≤ 250000.
// Длина каждой дороги ≤ 10000.
// Время работы O((N + M)log N), доп. память O(N + M)

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

class IGraph {
 public:

  struct Edge {
    Edge(int next, int weight):next(next), weight(weight) {}
    int next;
    int weight;
  };

  virtual ~IGraph() = default;

  // Добавление ребра от from к to.
  virtual void AddEdge(int from, int to, int weight) = 0;

  virtual int VerticesCount() const = 0;

  virtual std::vector<Edge> GetNextVertices(int vertex) const = 0;
};

class ListGraph : public IGraph {
 public:

  ListGraph() = default;

  explicit ListGraph(int size);

  explicit ListGraph(const IGraph *graph);

  ~ListGraph() override = default;

  void AddEdge(int from, int to, int weight) override;

  int VerticesCount() const override;

  std::vector<Edge> GetNextVertices(int vertex) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<Edge>> adjacency_list_;
};

int ShortestPathLength(const IGraph *graph, int from, int to);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new ListGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to, weight;
    std::cin >> from >> to >> weight;
    graph->AddEdge(from, to, weight);
    graph->AddEdge(to, from, weight);
  }
  int vertex_from, vertex_to;
  std::cin >> vertex_from >> vertex_to;
  std::cout << ShortestPathLength(graph, vertex_from, vertex_to) << std::endl;
  delete graph;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

ListGraph::ListGraph(int size)
    : adjacency_list_(size, std::vector<Edge>()) {}

ListGraph::ListGraph(const IGraph *graph)
    : adjacency_list_(graph->VerticesCount(), std::vector<Edge>()) {

  for (int i = 0; i < graph->VerticesCount(); ++i) {
    adjacency_list_[i] = graph->GetNextVertices(i);
  }
}

void ListGraph::AddEdge(int from, int to, int weight) {
  adjacency_list_[from].emplace_back(to, weight);
}

int ListGraph::VerticesCount() const {
  return adjacency_list_.size();
}

std::vector<IGraph::Edge> ListGraph::GetNextVertices(int vertex) const {
  return adjacency_list_[vertex];
}

///////////////////////////////////////////////////////////////////////////////

int ShortestPathLength(const IGraph *graph, int from, int to) {
  // первый член в паре - дистанция, второй - номер вершины
  std::set<std::pair<int, int>> priority_queue;
  std::vector<int> distances(graph->VerticesCount(), INT32_MAX);
  std::vector<int> visited(graph->VerticesCount(), 0);
  distances[from] = 0;
  visited[from] = 1;
  priority_queue.emplace(distances[from], from);
  while (!priority_queue.empty()) {
    auto vertex = *priority_queue.begin();
    for (auto next: graph->GetNextVertices(vertex.second)) {
      if (visited[next.next] == 0) {

        visited[next.next] = 1;
        distances[next.next] = distances[vertex.second] + next.weight;
        priority_queue.emplace(distances[next.next], next.next);

      } else if (distances[next.next] >
                                       distances[vertex.second] + next.weight) {

        priority_queue.erase(std::make_pair(distances[next.next], next.next));
        distances[next.next] = distances[vertex.second] + next.weight;
        priority_queue.emplace(distances[next.next], next.next);
      }
    }
    priority_queue.erase(vertex);
  }
  return distances[to];
}
