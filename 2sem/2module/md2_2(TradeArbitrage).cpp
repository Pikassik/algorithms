// B. Trade Arbitrage
// Необходимо написать торгового советника для поиска арбитража.
//
// Арбитраж - это торговля по цепочке различных валют в надежде заработать
// на небольших различиях в коэффициентах. Например, есть следующие курсы валют:
// GBP/USD: 0.67
// RUB/GBP: 78.66
// USD/RUB: 0.02
//
// Имея 1$ и совершив цикл USD->GBP->RUB->USD, получим 1.054$.
// Таким образом заработав 5.4
// Время работы O(V + E). Доп. память O(V + E).

#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <algorithm>
#include <unordered_set>

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

  virtual std::vector<Edge> GetPrevVertices(int vertex) const = 0;
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

  void Transpose();

  std::vector<class IGraph<Weight>::Edge> GetNextVertices(int vertex)
                                                                 const override;
  std::vector<class IGraph<Weight>::Edge> GetPrevVertices
                                                    (int vertex) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<class IGraph<Weight>::Edge>> adjacency_list_next_;

  //массив списков входящих ребер
  std::vector<std::vector<class IGraph<Weight>::Edge>> adjacency_list_prev_;
};

// Класс для поиска компонент сильной связанности. Реализация алгоритма Косарайю.
template<class Weight>
class GetSCC {
 public:
  explicit GetSCC(const IGraph<Weight> *graph);
  ~GetSCC();

  IGraph<Weight> *graph_;

  // Массив топологически отсортированного графа
  std::vector<int> order_stack_;

  std::vector<std::unordered_set<int>> GetComponents();

  // Массив сильносвязанный компонент
  std::vector<std::vector<int>> strongly_connected_components_;

  void TopologicalSort();

  // Поиск компонент сильной связанности и ребер между ними в топологически
  // отсортированном графе
  void SccSearching();
};

bool IsArbitration(const IGraph<double> *graph);

bool IsArbitrationInSCC(const IGraph<double> *graph, std::unordered_set<int>& scc);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count;
  std::cin >> vertices_count;
  IGraph<double> *graph = new ListGraph<double>(vertices_count);
  for (int from = 0; from < vertices_count; ++from) {
    for (int to = 0; to < vertices_count; ++to) {
      if (from == to) {
        continue;
      }
      double weight;
      std::cin >> weight;
      if (weight != -1) {
        graph->AddEdge(from, to, weight);
      }
    }
  }
  std::cout << (IsArbitration(graph) ? "YES" : "NO") << std::endl;
  delete graph;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

template<class Weight>
ListGraph<Weight>::ListGraph(int size)
    : adjacency_list_next_(size, std::vector<class IGraph<Weight>::Edge>())
      , adjacency_list_prev_(size, std::vector<class IGraph<Weight>::Edge>()) {}

template<class Weight>
ListGraph<Weight>::ListGraph(const IGraph<Weight> *graph)
    : adjacency_list_next_(graph->VerticesCount(),
                    std::vector<class IGraph<Weight>::Edge>())
    , adjacency_list_prev_(graph->VerticesCount(),
                    std::vector<class IGraph<Weight>::Edge>()) {

  for (int i = 0; i < graph->VerticesCount(); ++i) {
    adjacency_list_next_[i] = graph->GetNextVertices(i);

    adjacency_list_prev_[i] = graph->GetPrevVertices(i);
  }
}

template<class Weight>
void ListGraph<Weight>::AddEdge(int from, int to, Weight weight) {
  adjacency_list_next_[from].emplace_back(to, weight);
  adjacency_list_prev_[to].emplace_back(from, weight);
}

template<class Weight>
int ListGraph<Weight>::VerticesCount() const {
  return adjacency_list_next_.size();
}

template<class Weight>
void ListGraph<Weight>::Transpose() {
  std::swap(adjacency_list_prev_, adjacency_list_next_);
}

template<class Weight>
std::vector<class IGraph<Weight>::Edge> ListGraph<Weight>::GetNextVertices
                                                            (int vertex) const {
  return adjacency_list_next_[vertex];
}

template<class Weight>
std::vector<class IGraph<Weight>::Edge> ListGraph<Weight>::GetPrevVertices
                                                            (int vertex) const {
  return adjacency_list_prev_[vertex];
}

///////////////////////////////////////////////////////////////////////////////

template<class Weight>
GetSCC<Weight>::GetSCC(const IGraph<Weight> *graph)
: graph_(new ListGraph<Weight>(graph)) {}

template<class Weight>
GetSCC<Weight>::~GetSCC() {
  delete graph_;
}

template<class Weight>
std::vector<std::unordered_set<int>> GetSCC<Weight>::GetComponents() {
  TopologicalSort();
  SccSearching();
  order_stack_.clear();

  std::vector<std::unordered_set<int>> sc_components;

  for (auto &sc_component: strongly_connected_components_) {
    std::unordered_set<int> scc;

    for (auto vertex: sc_component) {
      scc.insert(vertex);
    }

    sc_components.push_back(scc);
  }

  strongly_connected_components_.clear();
  return sc_components;
}

template<class Weight>
void GetSCC<Weight>::TopologicalSort() {

  // 0 white
  // 1 grey
  // 2 black
  std::vector<int> color(graph_->VerticesCount(), 0);
  std::stack<int> stack;

  // DFS
  for (int i = 0; i < graph_->VerticesCount(); ++i) {
    if (color[i] == 0) {
      color[i] = 1;
      stack.push(i);

      for (auto next: graph_->GetNextVertices(i)) {
        if (color[next.next] == 0) {
          stack.push(next.next);
        }
      }

      while (!stack.empty()) {
        int current = stack.top();
        if (color[current] == 0) {
          color[current] = 1;

          for (auto next: graph_->GetNextVertices(current)) {
            if (color[next.next] == 0) {
              stack.push(next.next);
            }
          }

        } else if (color[current] == 1) {
          color[current] = 2;
          order_stack_.push_back(current);
          stack.pop();
        } else {
          stack.pop();
        }
      }
    }
  }
  std::reverse(order_stack_.begin(), order_stack_.end());
}

template<class Weight>
void GetSCC<Weight>::SccSearching() {

  // 0 white
  // 1 grey
  // 2 black
  std::vector<int> color(graph_->VerticesCount(), 0);
  std::stack<int> stack;

  dynamic_cast<ListGraph<Weight>*>(graph_)->Transpose();

  // DFS в порядке топологической сортировки
  for (auto vertex: order_stack_) {

    if (color[vertex] == 0) {
      std::vector<int> scc;
      std::vector<int> scc_next;

      color[vertex] = 1;
      stack.push(vertex);

      for (auto next: graph_->GetNextVertices(vertex)) {
        if (next.next != vertex) { // Проверка на петлю, чтобы не покрасить в черный
          stack.push(next.next);   // вершину раньше выхода из нее
        }
      }

      while (!stack.empty()) {
        int current = stack.top();
        if (color[current] == 0) {
          color[current] = 1;
          for (auto next: graph_->GetNextVertices(current)) {
            if (color[next.next] != 1) {
              stack.push(next.next);
            }
          }

        } else if (color[current] == 1) {
          color[current] = 2;
          scc.push_back(current);
          stack.pop();
        } else {
          scc_next.push_back(current);
          stack.pop();
        }
      }

      strongly_connected_components_.push_back(scc);
    }

  }
}

bool IsArbitration(const IGraph<double> *graph) {

  // разбиение графа на компоненты сильной связанности
  GetSCC<double> get_scc(graph);
  std::vector<std::unordered_set<int>> sc_components = get_scc.GetComponents();

  // Поиск арбитрижа в каждой компоненте связанности
  for (auto& scc: sc_components) {
    if (IsArbitrationInSCC(graph, scc)) {
      return true;
    }
  }

  return false;
}

bool IsArbitrationInSCC(const IGraph<double> *graph,
                                                 std::unordered_set<int>& scc) {
  std::vector<double> distances(graph->VerticesCount(), -1);
  distances[*scc.begin()] = 1;
  for (int i = 1; i < static_cast<int>(scc.size()); ++i) {
    for (auto vertex: scc) {
      for (auto edge: graph->GetNextVertices(vertex)) {
        if (scc.find(edge.next) != scc.end()) {
          if (distances[edge.next] < distances[vertex] * edge.weight) {
            distances[edge.next] = distances[vertex] * edge.weight;
          }
        }
      }
    }
  }

  for (auto vertex: scc) {
    for (auto edge: graph->GetNextVertices(vertex)) {
      if (scc.find(edge.next) != scc.end()) {
        if (distances[edge.next] < distances[vertex] * edge.weight) {
          return true;
        }
      }
    }
  }

  return false;
}
