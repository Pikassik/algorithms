// Задача № 4. Двудольный граф (2 балла)
// Дан невзвешенный неориентированный граф. Определить, является ли
// он двудольным. Требуемая сложность O(V+E).
// Ввод: v:кол-во вершин(макс. 50000), n:кол-во ребер(макс. 200000),
// n пар реберных вершин. Время O(V + E) Память O(V + E)
#include <iostream>
#include <unordered_set>
#include <vector>
#include <stack>

struct IGraph {
  virtual ~IGraph() = default;

  // Добавление ребра от from к to.
  virtual void AddEdge(int from, int to) = 0;

  virtual int VerticesCount() const  = 0;

  virtual void GetNextVertices(int vertex, std::vector<int>& vertices) const = 0;
  virtual void GetPrevVertices(int vertex, std::vector<int>& vertices) const = 0;
};

class SetGraph : public IGraph {
 public:
  SetGraph() = default;

  explicit SetGraph(int size);

  explicit SetGraph(const IGraph *graph);

  ~SetGraph() override = default;

  void AddEdge(int from, int to) override;

  int VerticesCount() const override;

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  // массив хеш-таблиц исходящих ребер
  std::vector<std::unordered_multiset<int>> buffer_next_;

  // массив хеш-таблиц входящих ребер
  std::vector<std::unordered_multiset<int>> buffer_prev_;
};

bool IsBipartiteGraph(const IGraph *graph);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new SetGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->AddEdge(from, to);
  	graph->AddEdge(to, from);
  }
  if (IsBipartiteGraph(graph)) {
    std::cout << "YES" << std::endl;
  } else {
    std::cout << "NO" << std::endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

SetGraph::SetGraph(int size)
    : buffer_next_(size, std::unordered_multiset<int>())
      , buffer_prev_(size, std::unordered_multiset<int>()) {}

SetGraph::SetGraph(const IGraph *graph)
    : buffer_next_(graph->VerticesCount(), std::unordered_multiset<int>())
      , buffer_prev_(graph->VerticesCount(), std::unordered_multiset<int>()) {
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    std::vector<int> vertices;

    // копирование исходящих ребер
    graph->GetNextVertices(i, vertices);
    for (auto x: vertices) {
      buffer_next_[i].insert(x);
    }

    // копирование входящих ребер
    graph->GetPrevVertices(i, vertices);
    for (auto x: vertices) {
      buffer_prev_[i].insert(x);
    }
  }
}

void SetGraph::AddEdge(int from, int to) {
  buffer_next_[from].insert(to);
  buffer_prev_[to].insert(from);
}

int SetGraph::VerticesCount() const {
  return static_cast<int>(buffer_next_.size());
}

void SetGraph::GetNextVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto x: buffer_next_[vertex]) {
    vertices.push_back(x);
  }
}

void SetGraph::GetPrevVertices(int vertex, std::vector<int> &vertices) const {
  vertices.clear();
  for (auto x: buffer_prev_[vertex]) {
    vertices.push_back(x);
  }
}

///////////////////////////////////////////////////////////////////////////////

bool IsBipartiteGraph(const IGraph *graph) {

  //0 - непосещенная вершина
  //1 - цвет 1
  //2 - цвет 2
  std::vector<int> color(graph->VerticesCount(), 0);

  // стек пар (вершина, из которой пришли)-(вершина)
  std::stack<std::pair<int, int>> stack;
  std::vector<int> next_vertices;

  //обход графа поиском в глубину
  for (int i = 0; i < graph->VerticesCount(); ++i) {
    if (color[i] == 0) {
      graph->GetNextVertices(i, next_vertices);
      color[i] = 1;

      // кладем на стек всех соседей данной вершины
      for (auto next: next_vertices) {
        stack.emplace(i, next);
      }

      // Если не были в вершине - красим в цвет, противоположный цвету предка.
      // если были, то если цвет совпадает с цветом предка, то это значит, что
      // граф не двусвязный, иначе просто извлекаем вершину и продолжаем
      // проверку.
      while (!stack.empty()) {
        std::pair<int, int> current = stack.top();
        if (color[current.second] == 0) {
          color[current.second] = 3 - color[current.first];
          graph->GetNextVertices(current.second, next_vertices);
          stack.pop();

          for (auto next: next_vertices) {
            if (next != current.first)
            stack.emplace(current.second, next);
          }
        } else if (color[current.first] == color[current.second]) {
          return false;
        } else {
          stack.pop();
        }
      }
    }
  }
  return true;
}