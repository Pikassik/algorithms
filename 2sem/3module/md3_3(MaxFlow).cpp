// B. Максимальный поток в неориентированном графе 2018
// Задан неориентированный граф, каждое ребро которого обладает целочисленной
// пропускной способностью. Найдите максимальный поток
// из вершины с номером 1 в вершину с номером n.
// (2≤n≤100, 1≤m≤1000). n - количество вершин, m - количество ребер
// Пропускные способности не превосходят 10^5.

#include <iostream>
#include <vector>
#include <memory>
#include <queue>
#include <optional>

template<class Weight>
struct IGraph {

  struct Edge {
    Edge(int next, Weight weight) : next(next), weight(weight) {}
    int next;
    Weight weight;
  };

  virtual ~IGraph() = default;

  // Добавление ребра от from к to.
  virtual void AddEdge(int from, int to, Weight weight) = 0;

  virtual void AddVertex() = 0;

  virtual int VerticesCount() const = 0;

  virtual std::vector<Edge> GetNextVertices(int vertex) const = 0;
};

template<class Weight>
class ListGraph : public IGraph<Weight> {
 public:
  ListGraph() = default;

  explicit ListGraph(int size);

  explicit ListGraph(const IGraph<Weight>* graph);

  ~ListGraph() override = default;

  void AddEdge(int from, int to, Weight weight) override;

  void AddVertex() override;

  int VerticesCount() const override;

  std::vector<class IGraph<Weight>::Edge>
  GetNextVertices(int vertex) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<class IGraph<Weight>::Edge>> adjacency_list_;
};

int MaxFlow(const std::shared_ptr<IGraph<int>>& graph,
            const std::vector<std::vector<int>>& capacities,
            int source,
            int target);

std::optional<std::vector<int>> FindPath(const std::shared_ptr<IGraph<int>>& graph,
                                         const std::vector<std::vector<int>>& capacities,
                                         const std::vector<std::vector<int>>& flow,
                                         int source,
                                         int target);

int FindBottleNeck(const std::vector<std::vector<int>>& capacities,
                   const std::vector<std::vector<int>>& flow,
                   const std::vector<int>& parent,
                   int source,
                   int target);

void UpdateFlow(std::vector<std::vector<int>>& flow,
                const std::vector<int>& parent,
                int bottleneck,
                int source,
                int target);

int GetFlowValue(const std::shared_ptr<IGraph<int>>& graph,
                 const std::vector<std::vector<int>>& flow,
                 int source);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  std::shared_ptr<IGraph<int>> graph =
      std::make_shared<ListGraph<int>>(vertices_count);
  std::vector<std::vector<int>> capacities(vertices_count,
                                           std::vector<int>(vertices_count, 0));

  for (int i = 0; i < edges_count; ++i) {
    int from, to, capacity;
    std::cin >> from >> to >> capacity;
    if (capacities[to - 1][from - 1] == 0) {
      graph->AddEdge(from - 1, to - 1, 0);
      graph->AddEdge(to - 1, from - 1, 0);
      capacities[from - 1][to - 1] = capacity;
    } else {
      // Если добавляю обратное ребро, то добавляю фиктивную вершину.

      graph->AddVertex();
      graph->AddEdge(from - 1, graph->VerticesCount() - 1, 0);
      graph->AddEdge(graph->VerticesCount() - 1, to - 1, 0);
      graph->AddEdge(to - 1, graph->VerticesCount() - 1, 0);
      graph->AddEdge(graph->VerticesCount() - 1, from - 1, 0);

      // Добавляем в каждую строчку таблички по нолику справа
      for (auto& arr: capacities) {
        arr.push_back(0);
      }

      // Добавляем строчку в конец
      capacities.emplace_back(graph->VerticesCount(), 0);

      capacities[from - 1][graph->VerticesCount() - 1] = capacity;
      capacities[graph->VerticesCount() - 1][to - 1] = capacity;
    }
  }

  std::cout << MaxFlow(graph, capacities, 0, vertices_count - 1) << std::endl;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

template<class Weight>
ListGraph<Weight>::ListGraph(int size)
    : adjacency_list_(size) {}

template<class Weight>
ListGraph<Weight>::ListGraph(const IGraph<Weight>* graph)
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
void ListGraph<Weight>::AddVertex() {
  adjacency_list_.emplace_back();
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

///////////////////////////////////////////////////////////////////////////////

int MaxFlow(const std::shared_ptr<IGraph<int>>& graph,
            const std::vector<std::vector<int>>& capacities,
            int source,
            int target) {
  std::vector<std::vector<int>>
      flow(graph->VerticesCount(), std::vector<int>(graph->VerticesCount(), 0));

  while (true) {
    std::optional<std::vector<int>>
        parent = FindPath(graph, capacities, flow, source, target);

    if (!parent) {
      break;
    }

    int bottleneck = FindBottleNeck(capacities, flow, *parent, source, target);
    UpdateFlow(flow, *parent, bottleneck, source, target);
  }

  return GetFlowValue(graph, flow, source);
}

std::optional<std::vector<int>> FindPath(const std::shared_ptr<IGraph<int>>& graph,
                                         const std::vector<std::vector<int>>& capacities,
                                         const std::vector<std::vector<int>>& flow,
                                         int source,
                                         int target) {
  std::queue<int> queue;
  std::vector<int> parent(graph->VerticesCount(), -1);
  parent[source] = 0;
  queue.push(source);

  bool flag = true;
  while (!queue.empty() && flag) {
    for (auto next: graph->GetNextVertices(queue.front())) {
      if ((capacities[queue.front()][next.next]
          - flow[queue.front()][next.next]) > 0 && parent[next.next] == -1) {
        queue.push(next.next);
        parent[next.next] = queue.front();
        if (next.next == target) {
          flag = false;
          break;
        }
      }
    }
    queue.pop();
  }

  if (parent[target] == -1) {
    return std::nullopt;
  }

  return parent;
}

int FindBottleNeck(const std::vector<std::vector<int>>& capacities,
                   const std::vector<std::vector<int>>& flow,
                   const std::vector<int>& parent,
                   int source,
                   int target) {
  int bottleneck = INT32_MAX;
  for (int current = target; current != source;
       current = parent[current]) {
    if (capacities[parent[current]][current] - flow[parent[current]][current]
        < bottleneck) {
      bottleneck = capacities[parent[current]][current]
          - flow[parent[current]][current];
    }
  }

  return bottleneck;
}

void UpdateFlow(std::vector<std::vector<int>>& flow,
                const std::vector<int>& parent,
                int bottleneck,
                int source,
                int target) {
  for (int current = target; current != source;
       current = parent[current]) {
    flow[parent[current]][current] += bottleneck;
    flow[current][parent[current]] -= bottleneck;
  }
}

int GetFlowValue(const std::shared_ptr<IGraph<int>>& graph,
                 const std::vector<std::vector<int>>& flow,
                 int source) {
  int flow_value = 0;
  for (auto next: graph->GetNextVertices(source)) {
    flow_value += flow[source][next.next];
  }
  return flow_value;
}
