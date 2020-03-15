// E. Планарный граф
// Дан невзвешенный неориентированный граф.
// Определить, является ли он планарным.
// Время работы O(V^3) Доп память O(V^2)
// Источник вдохновения https://github.com/tehnik819/Gamma-Algorithm-Java

#include <iostream>
#include <vector>
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

  void GetNextVertices(int vertex, std::vector<int> &vertices) const override;
  void GetPrevVertices(int vertex, std::vector<int> &vertices) const override;
 private:

  //массив списков исходящих ребер
  std::vector<std::vector<int>> buffer_next_;

  //массив списков входящих ребер
  std::vector<std::vector<int>> buffer_prev_;
};

class PlanarityTest {
 public:
  explicit PlanarityTest(IGraph *graph);
  ~PlanarityTest();
  bool GammaAlgorithm();
 private:
  IGraph *graph_;

  // пометка уложенных ребер как уложенные
  void LayChain(std::vector<std::vector<int>> &laid_edges,
                std::vector<int> &chain,
                bool cyclic);

  // алгоритм поиска сегментов
  std::vector<IGraph*> GetSegments(std::vector<int> &laid_vertices,
                                    std::vector<std::vector<int>> &laid_edges);

  // рекурсивная DFS-составляющая алгоритма поиска сегментов
  void DfsSegments(std::vector<int> &used,
                   std::vector<int> &laid_vertices,
                   std::vector<std::vector<int>> &added_edges,
                   IGraph *graph,
                   int vertex);

  // подсчет количества граней, который содержат данный сегмент
  std::vector<int> CalcNumOfFacesContainedSegments
      (std::vector<std::vector<int>> &interior_faces,
       std::vector<int> &external_face,
       std::vector<IGraph*> &segments,
       std::vector<int> &laid_vertices,
       std::vector<std::vector<int>> &dest_faces);

  // проверка на наличие сегмента в данной грани
  bool IsFaceContainsSegment(const std::vector<int> &face,
                             const IGraph *segment,
                             std::vector<int> &laid_vertices);

  // алгоритм поиска цепи в сегменте
  std::vector<int> GetChain(IGraph *segment, std::vector<int> &laid_vertices);

  // DFS-составляющая алгоритма поиска цепи в сегменте
  void DfsChain(IGraph *segment,
                std::vector<int> &used,
                std::vector<int> &laid_vertices,
                std::vector<int> &chain,
                int vertex);

  // алгоритм поиска цикла в графе
  std::vector<int> CycleSearching();

  // DFS-составляющая алгоритма поиска цикла в графе
  bool DfsCycle(std::vector<int> &cycle,
                std::vector<int> &used,
                int parent,
                int vertex);

  // освобождение памяти, выделенной для сегментов
  void FreeGraphs(std::vector<IGraph*> graphs);
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  int vertices_count, edges_count;
  std::cin >> vertices_count >> edges_count;
  IGraph *graph = new ListGraph(vertices_count);
  for (int i = 0; i < edges_count; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->AddEdge(from, to);
    graph->AddEdge(to, from);
  }
  PlanarityTest test(graph);
  std::cout << (test.GammaAlgorithm() ? "YES" : "NO");
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

PlanarityTest::PlanarityTest(IGraph *graph) {
  graph_ = new ListGraph(graph);
}

PlanarityTest::~PlanarityTest() {
  delete graph_;
}

bool PlanarityTest::GammaAlgorithm() {
  // не может быть подграфов гомеоморфных K_3,3 K_5
  if (graph_->VerticesCount() < 5) {
    return true;
  }

  // поиск цикла
  std::vector<int> cycle = CycleSearching();

  // если не нашли цикл, то это дерево
  if (cycle.empty()) {
    return true;
  }

  // массивы граней
  std::vector<std::vector<int>> interior_faces;
  std::vector<int> external_face = cycle;
  interior_faces.push_back(cycle);
  interior_faces.push_back(external_face);

  // массив и матрица уложенных вершин и ребер
  std::vector<int> laid_vertices(graph_->VerticesCount(), 0);
  std::vector<std::vector<int>> laid_edges
      (graph_->VerticesCount(), std::vector<int>(graph_->VerticesCount(), 0));

  // укладка вершин цикла
  for (int vertex: cycle) {
    laid_vertices[vertex] = 1;
  }

  // укладка ребер цикла
  LayChain(laid_edges, cycle, true);

  // поиск сегментов, цепей и их укладка
  while (true) {
    // поиск сегментов
    std::vector<IGraph*> segments = GetSegments(laid_vertices, laid_edges);

    // если сегментов нет, то граф уложен на плоскость
    if (segments.empty()) {
      FreeGraphs(segments);
      return true;
    }

    // массив граней, в которые умещается каждая грань
    std::vector<std::vector<int>>
        dest_faces(segments.size(), std::vector<int>());

    // подсчет количества граней, вмещающих сегмент
    std::vector<int> count = CalcNumOfFacesContainedSegments(interior_faces,
        external_face, segments, laid_vertices, dest_faces);

    // поиск минимального по количеству вмещающих граней сегмента
    int min_calc = 0;
    for (int i = 0; i < segments.size(); ++i) {
      if (count[i] < count[min_calc])
        min_calc = i;
    }

    // если есть сегмент, который не вмещает ни одна грань,
    // то мы не смогли уложить граф на плоскость
    if (count[min_calc] == 0) {
      FreeGraphs(segments);
      return false;

    } else {

      // поиск цепи
      std::vector<int> chain = GetChain(segments[min_calc], laid_vertices);

      // укладка вершин цепи
      for (int vertex: chain) {
        laid_vertices[vertex] = 1;
      }

      // укладка ребер цепи
      LayChain(laid_edges, chain, false);

      std::vector<int> face = dest_faces[min_calc];

      std::vector<int> face1;
      std::vector<int> face2;

      int contact_first = 0;
      int contact_second = 0;

      // поиск контактных вершин
      for (int i = 0; i < face.size(); ++i) {
        if (face[i] == chain.front()) {
          contact_first = i;
        }
        if (face[i] == chain.back()) {
          contact_second = i;
        }
      }

      std::vector<int> reversed_chain = chain;
      std::reverse(reversed_chain.begin(), reversed_chain.end());

      // разрезание грани и укладка цепи в новые грани
      if (face != external_face) {

        // разрезание грани
        if (contact_first < contact_second) {
          face1 = chain;
          for (int i = (contact_second + 1) % face.size(); i != contact_first;
               i = (i + 1) % face.size()) {
            face1.push_back(face[i]);
          }
          face2 = reversed_chain;
          for (int i = (contact_first + 1) % face.size(); i != contact_second;
               i = (i + 1) % face.size()) {
            face2.push_back(face[i]);
          }
        } else {
          face1 = reversed_chain;
          for (int i = (contact_first + 1) % face.size(); i != contact_second;
               i = (i + 1) % face.size()) {
            face1.push_back(face[i]);
          }
          face2 = chain;
          for (int i = (contact_second + 1) % face.size(); i != contact_first;
               i = (i + 1) % face.size()) {
            face2.push_back(face[i]);
          }
        }

        // извлечение старой грани
        interior_faces.erase(std::find(interior_faces.begin(),
                                       interior_faces.end(),
                                       face));

        //добавление новых граней
        interior_faces.push_back(face1);
        interior_faces.push_back(face2);

        //аналогичные действия для внешней грани
      } else {
        std::vector<int> new_external_face;
        if (contact_first < contact_second) {
          new_external_face = chain;
          for (int i = (contact_second + 1) % face.size(); i != contact_first;
               i = (i + 1) % face.size()) {
            new_external_face.push_back(face[i]);
          }
          face2 = chain;
          for (int i = (contact_second - 1 + face.size()) % face.size();
               i != contact_first; i = (i - 1 + face.size()) % face.size()) {
            face2.push_back(face[i]);
          }
        } else {
          new_external_face = reversed_chain;
          for (int i = (contact_first + 1) % face.size(); i != contact_second;
               i = (i + 1) % face.size()) {
            new_external_face.push_back(face[i]);
          }
          face2 = reversed_chain;
          for (int i = (contact_first - 1 + face.size()) % face.size();
               i != contact_second; i = (i - 1 + face.size()) % face.size()) {
            face2.push_back(face[i]);
          }
        }

        interior_faces.erase(std::find(interior_faces.begin(),
                                       interior_faces.end(),
                                       external_face));;

        interior_faces.push_back(new_external_face);
        interior_faces.push_back(face2);
        external_face = new_external_face;
      }
    }
    FreeGraphs(segments);
  }
}

void PlanarityTest::LayChain(std::vector<std::vector<int>> &laid_edges,
                             std::vector<int> &chain,
                             bool cyclic) {
  for (int i = 0; i < chain.size() - 1; ++i) {
    laid_edges[chain[i]][chain[i + 1]] = 1;
    laid_edges[chain[i + 1]][chain[i]] = 1;
  }

  if (cyclic) {
    laid_edges[chain.front()][chain.back()] = 1;
    laid_edges[chain.back()][chain.front()] = 1;
  }
}

std::vector<IGraph *> PlanarityTest::GetSegments(std::vector<int> &laid_vertices,
                                    std::vector<std::vector<int>> &laid_edges) {
  std::vector<IGraph *> segments;
  std::vector<int> next_vertices;

  // учет добавленных ребер(из-за интерфейса графа)
  std::vector<std::vector<int>> added_edges(graph_->VerticesCount(),
                                  std::vector<int>(graph_->VerticesCount(), 0));

  // поиск однореберных сегментов
  for (int current = 0; current < graph_->VerticesCount(); ++current) {
    graph_->GetNextVertices(current, next_vertices);
    for (auto next: next_vertices) {
      if (!laid_edges[current][next] && laid_vertices[current]
          && laid_vertices[next] && !added_edges[current][next]) {
        IGraph *graph = new ListGraph(graph_->VerticesCount());
        graph->AddEdge(current, next);
        graph->AddEdge(next, current);
        added_edges[current][next] = 1;
        added_edges[next][current] = 1;
        segments.push_back(graph);
      }
    }
  }

  std::vector<int> used(graph_->VerticesCount(), 0);

  for (int i = 0; i < graph_->VerticesCount(); ++i) {
    if (used[i] == 0 && !laid_vertices[i]) {
      IGraph *graph = new ListGraph(graph_->VerticesCount());
      DfsSegments(used, laid_vertices, added_edges, graph, i);
      segments.push_back(graph);
    }
  }

  return segments;
}

void PlanarityTest::DfsSegments(std::vector<int> &used,
                                std::vector<int> &laid_vertices,
                                std::vector<std::vector<int>> &added_edges,
                                IGraph *graph,
                                int vertex) {
  used[vertex] = 1;
  std::vector<int> next_vertices;
  graph_->GetNextVertices(vertex, next_vertices);
  for (auto next: next_vertices) {
    if (!added_edges[vertex][next]) {
      graph->AddEdge(vertex, next);
      graph->AddEdge(next, vertex);
      added_edges[vertex][next] = 1;
      added_edges[next][vertex] = 1;
    }
    if (used[next] == 0 && !laid_vertices[next]) {
      DfsSegments(used, laid_vertices, added_edges, graph, next);
    }
  }
}

std::vector<int> PlanarityTest::CalcNumOfFacesContainedSegments
    (std::vector<std::vector<int>> &interior_faces,
     std::vector<int> &external_face,
     std::vector<IGraph *> &segments,
     std::vector<int> &laid_vertices,
     std::vector<std::vector<int>> &dest_faces) {

  std::vector<int> count(segments.size(), 0);
  for (int i = 0; i < segments.size(); ++i) {
    for (auto &face: interior_faces) {
      if (IsFaceContainsSegment(face, segments[i], laid_vertices)) {
        dest_faces[i] = face;
        ++count[i];
      }
    }

    if (IsFaceContainsSegment(external_face, segments[i], laid_vertices)) {
      dest_faces[i] = external_face;
      ++count[i];
    }
  }
  return count;
}

bool PlanarityTest::IsFaceContainsSegment(const std::vector<int> &face,
                                          const IGraph *segment,
                                          std::vector<int> &laid_vertices) {
  // если в сегменте есть уложенная вершина,
  // не содержащаяся в грани, то сегмент не укладывается в грань
  std::vector<int> next_vertices;
  for (int current = 0; current < graph_->VerticesCount(); ++current) {
    segment->GetNextVertices(current, next_vertices);
    for (auto next: next_vertices) {
      if ((laid_vertices[current]
          && (std::find(face.begin(), face.end(), current) == face.end()))
          || (laid_vertices[next]
          && (std::find(face.begin(), face.end(), next) == face.end())))
        return false;
    }
  }
  return true;
}

std::vector<int> PlanarityTest::GetChain(IGraph *segment,
                                         std::vector<int> &laid_vertices) {
  std::vector<int> chain;
  std::vector<int> next_vertices;

  for (int i = 0; i < segment->VerticesCount(); ++i) {
    if (laid_vertices[i]) {
      segment->GetNextVertices(i, next_vertices);

      if (!next_vertices.empty()) {
        std::vector<int> used(segment->VerticesCount());
        DfsChain(segment, used, laid_vertices, chain, i);
        break;
      }
    }
  }
  return chain;
}

void PlanarityTest::DfsChain(IGraph *segment,
                             std::vector<int> &used,
                             std::vector<int> &laid_vertices,
                             std::vector<int> &chain,
                             int vertex) {
  used[vertex] = 1;
  chain.push_back(vertex);
  std::vector<int> next_vertices;
  segment->GetNextVertices(vertex, next_vertices);
  for (auto next: next_vertices) {
    if (used[next] == 0) {
      if (!laid_vertices[next]) {
        DfsChain(segment, used, laid_vertices, chain, next);
      } else {
        chain.push_back(next);
      }
      return;
    }
  }
}

std::vector<int> PlanarityTest::CycleSearching() {
  std::vector<int> cycle;
  std::vector<int> used(graph_->VerticesCount());
  bool has_cycle = DfsCycle(cycle, used, -1, 0);
  if (!has_cycle) {
    return std::vector<int>();
  }
  return cycle;
}

bool PlanarityTest::DfsCycle(std::vector<int> &cycle,
                             std::vector<int> &used,
                             int parent,
                             int vertex) {
  used[vertex] = 1;
  std::vector<int> next_vertices;
  graph_->GetNextVertices(vertex, next_vertices);
  for (auto next: next_vertices) {
    if (next == parent) {
      continue;
    }
    if (used[next] == 0) {
      cycle.push_back(vertex);
      // если цикл найден, то возвращаемся из рекурсии,
      // иначе выкидываем последний элемент
      if (DfsCycle(cycle, used, vertex, next)) {
        return true;
      } else {
        cycle.pop_back();
      }
    }

    // нашли цикл
    if (used[next] == 1) {
      cycle.push_back(vertex);
      std::vector<int> true_cycle;

      // копируем цикл, начиная с вершины, в которую попали
      for (auto current: cycle) {
        if (cycle[current] == next) {
          for (int i = current; i < cycle.size(); ++i) {
            true_cycle.push_back(cycle[i]);
          }
          cycle = true_cycle;
          return true;
        }
      }
      return true;
    }
  }

  used[vertex] = 2;
  return false;
}

void PlanarityTest::FreeGraphs(std::vector<IGraph*> graphs) {
  for (auto graph: graphs) {
    delete graph;
  }
}
