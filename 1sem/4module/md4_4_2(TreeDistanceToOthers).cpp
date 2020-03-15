// 4_2. Сумма длин до остальных.
// Для каждой вершины определите сумму расстояний до всех остальных вершин.
// Время работы должно быть O(n). Время работы O(n), доп память O(n).

#include <iostream>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

class Tree {
 public:
  Tree();                                   //Создает корень дерева

  ~Tree() = default;

  void Insert(int parent, int child);       // Добавление вершины в дерево

  vector<int> DistanceSum();                // Выводит сумму расстояний до всех
                                            // остальных вершин для всех вершин
 private:
  struct Node {
    Node(int parent);
    ~Node() = default;
    vector<int> children;                   // Массив индексов детей
    int parent = 0;                         // Индекс родителя
    int weight = 0;                         // Количество вершин в поддереве
    int children_distance = 0;              // Сумма расстояний до всех вершин
  };                                        // поддерева

  vector<Node> buffer_;

  void WeightCounter();            // Вычисляет количество вершин в поддереве
                                   // для всего дерева.
  void ChildrenDistanceCounter();  // Вычисляет сумму
};                                 // расстояний всех вершин поддерева

int main() {
  int n;
  cin >> n;
  Tree tree;
  for (size_t i = 0; i < n - 1; ++i) {
    int parent, child;
    cin >> parent >> child;
    tree.Insert(parent, child);
  }
  for (auto current: tree.DistanceSum()) {
    cout << current << endl;
  }
  return 0;
}

Tree::Tree()
    : buffer_(1, 0) {}

void Tree::Insert(int parent, int child) {
  buffer_.emplace_back(parent);
  buffer_[parent].children.push_back(child);
}

vector<int> Tree::DistanceSum() {
  WeightCounter();                // Подсчет весов и сумм расстояний
  ChildrenDistanceCounter();      // до вершин поддерева
  vector<int> distance_sum(buffer_.size(), 0);

  // Суммой расстояний для корня будет сумма расстояний до всех детей поддерева,
  // так как родителя у него нет. Для остальных же вершин она будет равна сумме
  // расстояний до детей поддерева и суммы расстояний до всех остальных, которая
  // равна сумме расстояний до всех вершин дерева от родителя без суммы
  // расстояний до вершин поддерева данной вершины, которая равна сумме
  // расстояний от вершины до вершин поддерева плюс вес поддерева данной
  // вершины, так как расстояние до каждой вершины поддерева от родителя на
  // единицу больше, а так же дополненная весом всего оставшегося дерева,
  // так как расстояние до каждой вершины на единицу больше, чем расстояние от
  // родителя.

  distance_sum[0] = buffer_[0].children_distance;
  for (size_t i = 1; i < buffer_.size(); ++i) {
    distance_sum[i] =
        buffer_[i].children_distance + ((distance_sum[buffer_[i].parent]
            - (buffer_[i].children_distance + buffer_[i].weight))
            + static_cast<int>(buffer_.size()) - buffer_[i].weight);
  }
  return distance_sum;
}

Tree::Node::Node(int parent)
    : parent(parent) {}

// Суммирует количество вершин в поддеревьях детей и добавляет себя.
// Суммы вершин в поддеревьях уже посчитаны, так как их индексы больше, а мы
// проходимся по массиву из конца в начало.
void Tree::WeightCounter() {
  for (auto it = buffer_.rbegin(); it < buffer_.rend(); ++it) {
    for (auto current: (*it).children) {
      (*it).weight += buffer_[current].weight;
    }
    ++(*it).weight;
  }
}


// Для каждой вершины добавляет сумму расстояний до детей каждого ребенка и вес
// ребенка, так как расстояние до каждый вершины для родителя на единицу больше.
// Суммы расстояний детей уже посчитаны, так как по построению их индексы
// больше, а мы проходимся по массиву из конца в начало.
void Tree::ChildrenDistanceCounter() {
  for (auto it = buffer_.rbegin(); it < buffer_.rend(); ++it) {
    for (auto current: (*it).children) {
      (*it).children_distance +=
          buffer_[current].children_distance + buffer_[current].weight;
    }
  }
}