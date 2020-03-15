// D. QueryProcess
// Задано дерево с корнем, содержащее (1 ≤ n ≤ 100 000) вершин,
// пронумерованных от 0 до n-1.
// Требуется ответить на m (1 ≤ m ≤ 10 000 000) запросов
// о наименьшем общем предке для пары вершин.
// Запросы генерируются следующим образом. Заданы числа a1, a2 и числа x, y и z.
// Числа a3, ..., a2m генерируются следующим образом:
// ai = (x ⋅ ai-2 + y ⋅ ai-1 + z) mod n.
// Первый запрос имеет вид (a1, a2). Если ответ на i-1-й запрос равен v,
// то i-й запрос имеет вид ((a2i-1 + v) mod n, a2i).
// Для решения задачи можно использовать метод двоичного подъёма.
// Время работы O((n + m)log(n)). Доп. память O(nlog(n)).
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

class Tree {
 public:
  Tree() = default;
  explicit Tree(size_t size);
  ~Tree() = default;
  void AddEdge(size_t from, size_t to);
  size_t VerticesCount() const;
  size_t GetParent(size_t vertex) const ;

 private:
  std::vector<size_t> parents_;
};

class LowestCommonAncestorHandler {
 public:
  LowestCommonAncestorHandler() = default;
  ~LowestCommonAncestorHandler() = default;
  explicit LowestCommonAncestorHandler(const Tree& tree);
  size_t QueryProcess(size_t first_vertex, size_t second_vertex) const;
 private:
  void InitTable(size_t max_depth);
  size_t InitDepths();
  std::vector<std::vector<size_t>> table_;
  std::vector<size_t> depths_;
  Tree tree_;
};

class QueryParams {
 public:
  QueryParams(size_t left_param,
              size_t right_param,
              size_t x_multiplier,
              size_t y_multiplier,
              size_t z_multiplier,
              size_t tree_size);
  size_t Left() const;
  size_t Right() const;
  void Update();
 private:
  size_t left_param_;
  size_t right_param_;
  size_t x_multiplier_;
  size_t y_multiplier_;
  size_t z_multiplier_;
  size_t tree_size_;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  size_t vertices_count, requests_count;
  std::cin >> vertices_count >> requests_count;

  Tree tree(vertices_count);
  for (size_t i = 1; i < vertices_count; ++i) {
    size_t parent;
    std::cin >> parent;
    tree.AddEdge(parent, i);
  }

  LowestCommonAncestorHandler lca_handler(tree);

  size_t left_param, right_param;
  std::cin >> left_param >> right_param;

  size_t x_multiplier, y_multiplier, z_multiplier;
  std::cin >> x_multiplier >> y_multiplier >> z_multiplier;

  QueryParams params_handler(left_param, right_param, x_multiplier,
      y_multiplier, z_multiplier, tree.VerticesCount());

  size_t requests_answers_sum = 0;
  size_t previous_request_answer = 0;
  for (int i = 0; i < requests_count; ++i) {
    previous_request_answer =
        lca_handler.QueryProcess((params_handler.Left() +
        previous_request_answer) % tree.VerticesCount(),
            params_handler.Right());
    params_handler.Update();
    requests_answers_sum += previous_request_answer;
  }

  std::cout << requests_answers_sum;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

size_t QueryParams::Left() const {
  return left_param_;
}

size_t QueryParams::Right() const {
  return right_param_;
}

void QueryParams::Update() {
  size_t new_left_param = (x_multiplier_ * left_param_ +
      y_multiplier_ * right_param_ + z_multiplier_) % tree_size_;
  size_t new_right_param = (x_multiplier_ * right_param_ +
      y_multiplier_ * new_left_param + z_multiplier_) % tree_size_;
  left_param_ = new_left_param;
  right_param_ = new_right_param;
}

QueryParams::QueryParams(size_t left_param,
                         size_t right_param,
                         size_t x_multiplier,
                         size_t y_multiplier,
                         size_t z_multiplier,
                         size_t tree_size)
    : left_param_(left_param)
      , right_param_(right_param)
      , x_multiplier_(x_multiplier)
      , y_multiplier_(y_multiplier)
      , z_multiplier_(z_multiplier)
      , tree_size_(tree_size) {}

Tree::Tree(size_t size)
    : parents_(size) {}

void Tree::AddEdge(size_t from, size_t to) {
  parents_[to] = from;
}

size_t Tree::VerticesCount() const {
  return parents_.size();
}

size_t Tree::GetParent(size_t vertex) const {
  return parents_[vertex];
}

LowestCommonAncestorHandler::LowestCommonAncestorHandler(const Tree& tree)
    : tree_(tree) {
  InitTable(InitDepths());
}

void LowestCommonAncestorHandler::InitTable(size_t max_depth) {
  size_t table_width = 1;
  if (max_depth != 0) {
    table_width = static_cast<size_t>(std::ceil(std::log2(max_depth)));
  }

  table_ = std::vector<std::vector<size_t>>(tree_.VerticesCount(),
      std::vector<size_t>(table_width, 0));

  for (size_t vertex = 0; vertex < tree_.VerticesCount(); ++vertex) {
    table_[vertex][0] = tree_.GetParent(vertex);
  }

  for (size_t i = 1; i < table_[0].size(); ++i) {
    for (size_t vertex = 0; vertex < tree_.VerticesCount(); ++vertex) {
      table_[vertex][i] = table_[table_[vertex][i - 1]][i - 1];
    }
  }
}

size_t LowestCommonAncestorHandler::InitDepths() {
  depths_.resize(tree_.VerticesCount());
  std::vector<size_t> stack;
  size_t max_depth = 0;

  for (size_t vertex = 1; vertex < tree_.VerticesCount(); ++vertex) {
    if (depths_[vertex] == 0) {
      stack.push_back(vertex);
      while (depths_[tree_.GetParent(stack.back())] == 0 &&
             tree_.GetParent(stack.back()) != 0) {
        stack.push_back(tree_.GetParent(stack.back()));
      }

      while(!stack.empty()) {
        depths_[stack.back()] = depths_[tree_.GetParent(stack.back())] + 1;
        max_depth = std::max(depths_[stack.back()], max_depth);
        stack.pop_back();
      }
    }
  }

  return max_depth;
}

size_t LowestCommonAncestorHandler::QueryProcess(size_t first_vertex,
                                        size_t second_vertex) const {
  int depth_difference = depths_[first_vertex] - depths_[second_vertex];
  if (depth_difference  < 0) {
    std::swap(first_vertex, second_vertex);
    depth_difference *= -1;
  }

  for (size_t i = 0; depth_difference != 0; ++i) {
    if (depth_difference % 2 == 1) {
      first_vertex = table_[first_vertex][i];
    }

    depth_difference /= 2;
  }

  if (first_vertex == second_vertex) {
    return first_vertex;
  }

  for (int i = table_[0].size() - 1; i >= 0; --i) {
    if (table_[first_vertex][i] != table_[second_vertex][i]) {
      first_vertex = table_[first_vertex][i];
      second_vertex = table_[second_vertex][i];
    }
  }

  return tree_.GetParent(first_vertex);
}
