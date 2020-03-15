// E. Супер-массив строк
// Реализуйте структуру данных “массив строк” на основе декартового дерева
// по неявному ключу со следующими методами:
//    // Добавление строки в позицию position.\\
//    // Все последующие строки сдвигаются на одну позицию вперед.\\
//    void InsertAt( int position, const std::string& value );\\
//    // Удаление строки из позиции position.\\
//    // Все последующие строки сдвигаются на одну позицию назад.\\
//    void DeleteAt( int position );\\
//    // Получение строки из позиции position.\\
//    std::string GetAt( int position );
//
// Все методы должны работать за O(log n) в среднем,
// где n – текущее количество строк в массиве.
// Время работы O(klog(k)). Доп. память O(klog(k)). k - количество запросов.
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <cstdint>

template<class T>
class Rope {
 public:
  Rope() = default;
  ~Rope() = default;
  void InsertAt(size_t position, const T& value);
  void DeleteAt(size_t position);
  void DeleteAt(size_t from, size_t to);
  T& GetAt(size_t position);
 private:
  struct Node {
    explicit Node(const T& value);
    std::unique_ptr<Node> left_child;
    std::unique_ptr<Node> right_child;
    size_t subtree_weight;
    std::uint_fast64_t priority;
    T value;
   private:
    static std::random_device random_device_;
    static std::mt19937_64 engine_;
    static std::uniform_int_distribution<std::uint_fast64_t> distribution_;
  };

  inline void UpdateWeight(std::unique_ptr<Node>&& node);
  inline size_t Weight(const std::unique_ptr<Node>& node) const;

  std::pair<std::unique_ptr<Node>&&, std::unique_ptr<Node>&&>
  Split(std::unique_ptr<Node> tree, size_t key);

  std::unique_ptr<Node>&& Merge(std::unique_ptr<Node>&& left_tree,
                                std::unique_ptr<Node>&& right_tree);

  std::string& RecursiveGetAt(std::unique_ptr<Node>&& node, size_t position);

  std::unique_ptr<Node> root_;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  Rope<std::string> rope;
  int commands_count;
  std::cin >> commands_count;
  for (int i = 0; i < commands_count; ++i) {
    char command;
    std::cin >> command;
    if (command == '+') {
      size_t position;
      std::string value;
      std::cin >> position >> value;
      rope.InsertAt(position, value);
    } else if (command == '-') {
      size_t from, to;
      std::cin >> from >> to;
      rope.DeleteAt(from, to);
    } else {
      size_t position;
      std::cin >> position;
      std::cout << rope.GetAt(position) << std::endl;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

template<class T>
void Rope<T>::InsertAt(size_t position, const T& value) {
  auto splitted_tree = Split(std::move(root_), position);

  root_ = Merge(Merge(std::move(splitted_tree.first),
                      std::make_unique<Node>(value)),
                std::move(splitted_tree.second));
}

template<class T>
void Rope<T>::DeleteAt(size_t position) {
  DeleteAt(position, position);
}

template<class T>
void Rope<T>::DeleteAt(size_t from, size_t to) {
  auto splitted_root = Split(std::move(root_), from);
  auto splitted_right_tree = Split(std::move(splitted_root.second),
                                   to - from + 1);
  root_ = Merge(std::move(splitted_root.first),
                std::move(splitted_right_tree.second));
}

template<class T>
T& Rope<T>::GetAt(size_t position) {
  return RecursiveGetAt(std::move(root_), position);
}

template<class T>
Rope<T>::Node::Node(const T& value)
    : subtree_weight(1)
      , priority(distribution_(engine_))
      , value(value) {}

template<class T>
std::random_device Rope<T>::Node::random_device_;

template<class T>
std::mt19937_64 Rope<T>::Node::engine_ =
    std::mt19937_64(Rope<T>::Node::random_device_());

template<class T>
std::uniform_int_distribution<std::uint_fast64_t> Rope<T>::Node::distribution_ =
    std::uniform_int_distribution<std::uint_fast64_t>(0, UINT64_MAX);

template<class T>
void Rope<T>::UpdateWeight(std::unique_ptr<Rope::Node>&& node) {
  node->subtree_weight = Weight(node->left_child) +
      Weight(node->right_child) + 1;
}

template<class T>
size_t Rope<T>::Weight(const std::unique_ptr<Rope::Node>& node) const {
  if (node) {
    return node->subtree_weight;
  } else {
    return 0;
  }
}

template<class T>
class std::pair<std::unique_ptr<struct Rope<T>::Node>&&,
                std::unique_ptr<struct Rope<T>::Node>&&>
Rope<T>::Split(std::unique_ptr<struct Rope<T>::Node> tree,
               size_t key) {
  if (tree == nullptr) {
    return std::make_pair(std::unique_ptr<Node>(), std::unique_ptr<Node>());
  }

  size_t left_subtree_weight = Weight(tree->left_child);
  if (key <= left_subtree_weight) {
    auto left_child_split = Split(std::move(tree->left_child), key);
    tree->left_child = std::move(left_child_split.second);
    UpdateWeight(std::move(tree));
    return std::make_pair(std::move(left_child_split.first), std::move(tree));
  }

  auto right_child_split = Split(std::move(tree->right_child),
      key - left_subtree_weight - 1);
  tree->right_child = std::move(right_child_split.first);
  UpdateWeight(std::move(tree));
  return std::make_pair(std::move(tree), std::move(right_child_split.second));
}

template<class T>
std::unique_ptr<struct Rope<T>::Node>&&
Rope<T>::Merge(std::unique_ptr<Rope::Node>&& left_tree,
               std::unique_ptr<Rope::Node>&& right_tree) {
  if(left_tree == nullptr) {
    return std::move(right_tree);
  }

  if (right_tree == nullptr) {
    return std::move(left_tree);
  }

  if (left_tree->priority > right_tree->priority) {
    left_tree->right_child = Merge(std::move(left_tree->right_child),
                                   std::move(right_tree));
    UpdateWeight(std::move(left_tree));
    return std::move(left_tree);
  }

  right_tree->left_child = Merge(std::move(left_tree),
                                 std::move(right_tree->left_child));
  UpdateWeight(std::move(right_tree));
  return std::move(right_tree);
}

template<class T>
std::string& Rope<T>::RecursiveGetAt(std::unique_ptr<Rope::Node>&& node,
                                              size_t position) {
  if (position == Weight(node->left_child)) {
    return node->value;
  }

  if (position < Weight(node->left_child)) {
    return RecursiveGetAt(std::move(node->left_child), position);
  } else {
    return RecursiveGetAt(std::move(node->right_child),
                          position - Weight(node->left_child) - 1);
  }
}
