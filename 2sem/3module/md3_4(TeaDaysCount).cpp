// C. Чай
// В одном из отделов крупной организации работает n человек.
// Как практически все сотрудники этой организации, они любят
// пить чай в перерывах между работой. При этом они достаточно
// дисциплинированы и делают в день ровно один перерыв, во время
// которого пьют чай. Для того, чтобы этот перерыв был максимально
// приятным, каждый из сотрудников этого отдела обязательно пьет чай
// одного из своих любимых сортов. В разные дни сотрудник может пить
// чай разных сортов. Для удобства пронумеруем сорта чая числами
// от 1 до m. Недавно сотрудники отдела купили себе большой набор
// чайных пакетиков, который содержит a1 пакетиков чая сорта номер
// 1, a2 пакетиков чая сорта номер 2, ..., am пакетиков чая сорта номер
// m. Теперь они хотят знать, на какое максимальное число дней им может
// хватить купленного набора так, чтобы в каждый из дней каждому из сотрудников
// доставался пакетик чая одного из его любимых сортов. Каждый сотрудник отдела
// пьет в день ровно одну чашку чая, которую заваривает из одного пакетика.
// При этом пакетики чая не завариваются повторно.
// Время работы O(V * E^2 * logK), где V = n + m + 2, E = n + m + f, где f -
// количество предпочтений у сотрудников, K - искомое количество дней,
// K <= [sum(ai) / n]

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

int GetMaxDaysCount(const std::shared_ptr<IGraph<int>>& graph,
                    const std::vector<std::vector<int>>& capacities,
                    int employee_count);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int employee_count, tea_count;
  std::cin >> employee_count >> tea_count;
  int graph_size = employee_count + tea_count + 2;
  std::shared_ptr<IGraph<int>> graph =
      std::make_shared<ListGraph<int>>(graph_size);
  std::vector<std::vector<int>> capacities(graph_size,
                                           std::vector<int>(graph_size, 0));
  
  for (int i = 0; i < tea_count; ++i) {
    int tea_capacity;
    int from = 1 + employee_count + i;
    int to = graph->VerticesCount() - 1;
    std::cin >> tea_capacity;
    graph->AddEdge(from, to, 0);
    graph->AddEdge(to, from, 0);
    capacities[from][to] = tea_capacity;
  }

  for (int i = 0; i < employee_count; ++i) {
    int preferred_tea_count;
    std::cin >> preferred_tea_count;
    for (int j = 0; j < preferred_tea_count; ++j) {
      int tea_number;
      std::cin >> tea_number;
      int from = 1 + i;
      int to = employee_count + tea_number;
      graph->AddEdge(from, to, 0);
      graph->AddEdge(to, from, 0);
      capacities[from][to] = INT32_MAX;
    }
  }

  for (int i = 0; i < employee_count; ++i) {
    graph->AddEdge(0, i + 1, 0);
    graph->AddEdge(i + 1, 0, 0);
  }

  std::cout << GetMaxDaysCount(graph, capacities, employee_count)
            << std::endl;

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

int GetMaxDaysCount(const std::shared_ptr<IGraph<int>>& graph,
                    const std::vector<std::vector<int>>& capacities,
                    int employee_count) {
  std::vector<std::vector<int>> modified_capacities = capacities;

  // Далее приведен бинпоиск по ответу

  int previous_days_count = 0;
  int days_count = 1;

  // Ищем промежуток, на котором нужно запустить бинпоиск
  while (true) {
    for (int i = 0; i < employee_count; ++i) {
      modified_capacities[0][i + 1] = days_count;
    }

    int flow_value = MaxFlow(graph, modified_capacities, 0, graph->VerticesCount() - 1);

    int expected_flow_value = days_count * employee_count;

    // Модифицированное сравнение (оператор <=)
    if (flow_value == expected_flow_value) {
      previous_days_count = days_count;
      days_count *= 2;
    } else {
      break;
    }
  }

  int left = previous_days_count;
  int right = days_count;

  // Запускаем бинпоиск на найденном промежутке
  while (left < right) {
    int middle = (right + left) / 2;

    for (int i = 0; i < employee_count; ++i) {
      modified_capacities[0][i + 1] = middle;
    }

    int flow_value = MaxFlow(graph, modified_capacities, 0, graph->VerticesCount() - 1);

    int expected_flow_value = middle * employee_count;

    if (flow_value == expected_flow_value) {
      left = middle + 1;
    } else {
      right = middle;
    }
  }

  return left - 1;
}
