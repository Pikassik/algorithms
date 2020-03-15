// F. Дополнение до сильносвязного
// Дан ориентированный граф. Определите, какое минимальное число ребер
// необходимо добавить, чтобы граф стал сильносвязным. В графе возможны петли.
// Время работы O(V + E), Доп. Память O(V + E)
#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>

struct IGraph {
  virtual ~IGraph() = default;

  // Добавление ребра от from к to.
  virtual void AddEdge(int from, int to) = 0;

  virtual int VerticesCount() const = 0;

  virtual void GetNextVertices(int vertex,
                               std::vector<int> &vertices) const = 0;
  virtual void GetPrevVertices(int vertex,
                               std::vector<int> &vertices) const = 0;
};

class ListGraph : public IGraph {
 public:
  ListGraph() = default;

  explicit ListGraph(int size);

  explicit ListGraph(const IGraph *graph);

  ~ListGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void Transpose();

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<int>> buffer_next_;

  //массив списков входящих ребер
  std::vector<std::vector<int>> buffer_prev_;
};

// Реализация алгоритма Косарайю
class EdgesToBecomeSC {
 private:
  friend int EdgesToBecomeStronglyConnected(const IGraph *graph);
  explicit EdgesToBecomeSC(const IGraph *graph);
  ~EdgesToBecomeSC();
  IGraph *graph_;
  IGraph *scc_graph_;

  // Массив топологически отсортированного графа
  std::vector<int> order_stack_;

  // Массив сильносвязанный компонент
  std::vector<std::vector<int>> strongly_connected_components_;

  // Массив ребер в сильно связанном графе
  std::vector<std::vector<int>> scc_next_vertices_;

  // Cоответствие вершины к сильносвязанной компоненте
  std::vector<int> vertex_to_scc_;

  void TopologicalSort();

  // Поиск компонент сильной связанности и ребер между ними в топологически
  // отсортированном графе
  void SccSearching();

  // Построение конденсированного графа
  void SccBuilding();

  // Подсчет необходимого количества ребер для дополнения до сильносвязанного
  int RequiredEdgesCount();
};

int EdgesToBecomeStronglyConnected(const IGraph *graph);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new ListGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->AddEdge(from - 1, to - 1);
  }
  std::cout << EdgesToBecomeStronglyConnected(graph);
  delete graph;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

ListGraph::ListGraph(int size)
    : buffer_next_(size, std::vector<int>())
      , buffer_prev_(size, std::vector<int>()) {}

ListGraph::ListGraph(const IGraph *graph)
    : buffer_next_(graph->VerticesCount(), std::vector<int>())
      , buffer_prev_(graph->VerticesCount(), std::vector<int>()) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> vertices;

    // копирование исходящих ребер
    graph->GetNextVertices(i, vertices);
    for (auto current: vertices) {
      buffer_next_[i].push_back(current);
    }

    // копирование входящих ребер
    graph->GetPrevVertices(i, vertices);
    for (auto current: vertices) {
      buffer_prev_[i].push_back(current);
    }
  }
}

void ListGraph::AddEdge(int from, int to) {
  buffer_next_[from].push_back(to);
  buffer_prev_[to].push_back(from);
}

int ListGraph::VerticesCount() const {
  return static_cast<int>(buffer_next_.size());
}

void ListGraph::Transpose() {
  std::swap(buffer_prev_, buffer_next_);
}

void ListGraph::GetNextVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_next_[vertex]) {
    vertices.push_back(current);
  }
}

void ListGraph::GetPrevVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto current: buffer_prev_[vertex]) {
    vertices.push_back(current);
  }
}

///////////////////////////////////////////////////////////////////////////////

EdgesToBecomeSC::EdgesToBecomeSC(const IGraph *graph)
    : scc_graph_(nullptr)
      , vertex_to_scc_(graph->VerticesCount(), -1) {
  graph_ = new ListGraph(graph);
}

EdgesToBecomeSC::~EdgesToBecomeSC() {
  delete graph_;
  delete scc_graph_;
}

void EdgesToBecomeSC::TopologicalSort() {

  // 0 white
  // 1 grey
  // 2 black
  std::vector<int> color(graph_->VerticesCount(), 0);
  std::stack<int> stack;
  std::vector<int> next_vertices;

  // DFS
  for (int i = 0; i < graph_->VerticesCount(); ++i) {
    if (color[i] == 0) {
      graph_->GetNextVertices(i, next_vertices);
      color[i] = 1;
      stack.push(i);

      for (auto next: next_vertices) {
        if (color[next] == 0) {
          stack.push(next);
        }
      }

      while (!stack.empty()) {
        int current = stack.top();
        if (color[current] == 0) {
          color[current] = 1;
          graph_->GetNextVertices(current, next_vertices);

          for (auto next: next_vertices) {
            if (color[next] == 0) {
              stack.push(next);
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

void EdgesToBecomeSC::SccSearching() {

  // 0 white
  // 1 grey
  // 2 black
  std::vector<int> color(graph_->VerticesCount(), 0);
  std::stack<int> stack;
  std::vector<int> next_vertices;

  dynamic_cast<ListGraph*>(graph_)->Transpose();

  // DFS в порядке топологической сортировки
  for (auto vertex: order_stack_) {

    if (color[vertex] == 0) {
      std::vector<int> scc;
      std::vector<int> scc_next;

      graph_->GetNextVertices(vertex, next_vertices);
      color[vertex] = 1;
      stack.push(vertex);

      for (auto next: next_vertices) {
        if (next != vertex) { // Проверка на петлю, чтобы не покрасить в черный
          stack.push(next);   // вершину раньше выхода из нее
        }
      }

      while (!stack.empty()) {
        int current = stack.top();
        if (color[current] == 0) {
          color[current] = 1;
          graph_->GetNextVertices(current, next_vertices);

          for (auto next: next_vertices) {
            if (color[next] != 1) {
              stack.push(next);
            }
          }

        } else if (color[current] == 1) {
          color[current] = 2;
          scc.push_back(current);
          vertex_to_scc_[current] = strongly_connected_components_.size();
          stack.pop();
        } else {
          // если попали в черную вершину, то это ребро конденсированного графа

          scc_next.push_back(current);
          stack.pop();
        }
      }

      strongly_connected_components_.push_back(scc);
      scc_next_vertices_.push_back(scc_next);
    }

  }
}

void EdgesToBecomeSC::SccBuilding() {
  scc_graph_ = new ListGraph(strongly_connected_components_.size());

  for (int i = 0; i < strongly_connected_components_.size(); ++i) {
    for (auto next: scc_next_vertices_[i]) {
      scc_graph_->AddEdge(vertex_to_scc_[next], i);
    }
  }
}

int EdgesToBecomeSC::RequiredEdgesCount() {
  int begins = 0;
  int ends = 0;
  std::vector<int> vertices;

  for (int i = 0; i < scc_graph_->VerticesCount(); ++i) {
    scc_graph_->GetPrevVertices(i, vertices);
    if (vertices.empty()) {
      ++begins;
    }

    scc_graph_->GetNextVertices(i, vertices);
    if (vertices.empty()) {
      ++ends;
    }
  }

  return (scc_graph_->VerticesCount() == 1) ? 0 : std::max(begins, ends);
}

int EdgesToBecomeStronglyConnected(const IGraph *graph) {
  EdgesToBecomeSC counter(graph);
  counter.TopologicalSort();
  counter.SccSearching();
  counter.SccBuilding();
  return counter.RequiredEdgesCount();
}