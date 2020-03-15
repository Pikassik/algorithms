// A. Минимальное остовное дерево 2018
// Дан неориентированный связный граф. Требуется найти вес
// минимального остовного дерева в этом графе.
// Вариант 2. С помощью алгоритма Крускала.
// n и m — количество вершин и ребер графа
// соответственно (1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000).
// Время работы O(m * log(n)). Доп. память O(n + m)

#include <iostream>
#include <vector>
#include <cstdint>
#include <set>
#include <numeric>

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

class DSU {
 public:
  DSU() = delete;
  DSU(size_t size);
  size_t Get(size_t element);
  void Union(size_t first, size_t second);
 protected:
  std::vector<size_t> buffer_;
  std::vector<int> ranks_;
};

uint64_t GetMSTWeight(const IGraph<int>* graph);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph<int> *graph = new ListGraph<int>(vertices_count);

  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    int weight;
    std::cin >> weight;
    graph->AddEdge(from - 1, to - 1, weight);
  }

  std::cout << GetMSTWeight(graph) << std::endl;

  delete graph;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

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

DSU::DSU(size_t size)
    : buffer_(size)
      , ranks_(size, 0) {
  std::iota(buffer_.begin(), buffer_.end(), 0);
}

size_t DSU::Get(size_t element) {
  if (buffer_[element] != element) {
    buffer_[element] = Get(buffer_[element]);
  }

  return buffer_[element];
}

void DSU::Union(size_t first, size_t second) {
  size_t get_first = Get(first);
  size_t get_second = Get(second);
  if (ranks_[get_first] == ranks_[get_second]) {
    ++ranks_[get_first];
  }

  if (ranks_[get_first] < ranks_[get_second]) {
    std::swap(get_first, get_second);
  }

  buffer_[get_second] = buffer_[get_first];
}

///////////////////////////////////////////////////////////////////////////////

uint64_t GetMSTWeight(const IGraph<int>* graph) {
  DSU components(graph->VerticesCount());

  // пары вида <weight, <first, second> >, их сортировка по весу
  std::set<std::pair<int, std::pair<int, int>>> edges;
  for (int current = 0; current < graph->VerticesCount(); ++current) {
    for (auto next: graph->GetNextVertices(current)) {
      edges.emplace(next.weight, std::make_pair(current, next.next));
    }
  }

  // алгоритм Крускала
  uint64_t total_weight = 0;
  for (auto edge: edges) {
    if (components.Get(edge.second.first) != components.Get(edge.second.second)) {
      components.Union(edge.second.first, edge.second.second);
      total_weight += edge.first;
    }
  }

  return total_weight;
}
