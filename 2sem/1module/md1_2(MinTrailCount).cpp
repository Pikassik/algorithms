#include <iostream>
#include <unordered_set>
#include <vector>
#include <stack>
#include <queue>

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

int MinTrailCount(const IGraph *graph, int u, int v);

///////////////////////////////////////////////////////////////////////////////

int main() {
  int u, v;
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new SetGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->AddEdge(from, to);
    graph->AddEdge(to, from);
  }
  std::cin >> u >> v;
  std::cout << MinTrailCount(graph, u, v);
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

int MinTrailCount(const IGraph *graph, int u, int v) {
  std::vector<int> depth_left(graph->VerticesCount(), -1);
  std::queue<std::pair<int, int>> queue;
  std::vector<int> next_vertices;
  int trail_length = 1000000;
  queue.emplace(u, u);

  while (!queue.empty()) {
    std::pair<int, int> current = queue.front();

    if (current.second == v) {
      trail_length = std::min(trail_length, depth_left[current.first] + 1);
    }

    if (depth_left[current.second] == -1) {
      depth_left[current.second] = depth_left[current.first] + 1;
      graph->GetNextVertices(current.second, next_vertices);

      for (auto next: next_vertices) {
        if (next != current.first && depth_left[next] < depth_left[current.second]) {
          queue.emplace(current.second, next);
        }
      }
      queue.pop();

    } else if (color[current.second] == 1) {
      cycle = std::min(cycle, depth[current.first] + depth[current.second] + 1);
      break;
    }
  }

  return (cycle != 100000 ? cycle : -1);
}