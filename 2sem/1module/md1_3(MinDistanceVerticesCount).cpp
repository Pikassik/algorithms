// C. Количество различных путей
// Дан невзвешенный неориентированный граф. В графе может быть несколько
// кратчайших путей между какими-то вершинами. Найдите количество различных
// кратчайших путей между заданными вершинами. Требуемая сложность O(V+E).
// Время работы O(V + E), доп. память O(V + E)
#include <iostream>
#include <vector>
#include <queue>
#include <list>

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

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<int>> buffer_next_;

  //массив списков входящих ребер
  std::vector<std::vector<int>> buffer_prev_;
};

int WaysCount(const IGraph *graph, int begin, int end);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count, begin, end;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new ListGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->AddEdge(from, to);
    graph->AddEdge(to, from);
  }
  std::cin >> begin >> end;
  std::cout << WaysCount(graph, begin, end);
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

int WaysCount(const IGraph *graph, int begin, int end) {

  // depth[i] == -1 => вершина еще не посещена
  // depth[i] != -1 => значение равно расстоянию от начальной вершины
  std::vector<int> depth(graph->VerticesCount(), -1);

  // ways_count == количество кратчайших путей из начальной до данной
  std::vector<int> ways_count(graph->VerticesCount(), 0);
  std::vector<int> next_vertices;
  std::queue<int> queue;

  queue.push(begin);
  ways_count[begin] = 1;

  // Обход в глубину графа из начальной вершины. Если мы не были в ней, то
  // ставим расстояние до нее и обновляем количество путей. Если мы были в
  // вершине, то если этот путь кратчайший (т.е. глубина вершины, из которой мы
  // пришли, на 1 меньше, чем та, в которую мы идем, depth[cur] == depth[next]
  // + 1), то добавляем к количеству кратчайших путей до нее кол-во кратчайших
  // путей до cur и кладем next в очередь. Иначе не кладем ее в очередь.
  while (!queue.empty()) {
    graph->GetNextVertices(queue.front(), next_vertices);

    for (auto next: next_vertices) {
      if (depth[next] == -1) {
        ways_count[next] = ways_count[queue.front()];
        depth[next] = depth[queue.front()] + 1;
        queue.push(next);
      } else if (depth[next] == depth[queue.front()] + 1) {
        ways_count[next] += ways_count[queue.front()];
      }
    }

    queue.pop();
  }

  return ways_count[end];
}

