//	Дано число N и последовательность из N целых чисел.
//  Найти вторую порядковую статистику на заданных диапазонах.
//	Для решения задачи используйте структуру данных Sparse Table.
//  Требуемое время обработки каждого диапазона O(1).
//  Время подготовки структуры данных O(nlogn)
//  Доп. память O(nlogn)

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_set>

class SparseTable {
 public:
  explicit SparseTable(const std::vector<int>& array);
  int SecondStatistics(size_t left_bound, size_t right_bound) const;
 private:
  void InitTable();
  void InitLogs();
  std::vector<std::vector<std::vector<int>>> table_;
  std::vector<int> buffer_;
  std::vector<size_t> logs_;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  int array_size, requests_count;
  std::cin >> array_size >> requests_count;

  std::vector<int> input_array;
  for (int i = 0; i < array_size; ++i) {
    int input;
    std::cin >> input;
    input_array.push_back(input);
  }

  SparseTable table(input_array);
  for (int i = 0; i < requests_count; ++i) {
    size_t left_bound, right_bound;
    std::cin >> left_bound >> right_bound;
    std::cout << table.SecondStatistics(left_bound - 1, right_bound - 1) << std::endl;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

SparseTable::SparseTable(const std::vector<int>& array)
    : table_(array.size(),
             std::vector<std::vector<int>>(
                 array.empty() ? 0 : static_cast<size_t>(
                     std::ceil(std::log2(static_cast<double>(array.size())))),
                 std::vector<int>(2, -1)))
      , buffer_(array)
      , logs_(array.size() + 1) {
  InitTable();
  InitLogs();
}

void SparseTable::InitTable() {
  for (size_t i = 0; i < buffer_.size(); ++i) {
    table_[i][0] = {static_cast<int>(i), -1};
  }

  size_t two_pow_j = 2;
  for (size_t j = 1; j < table_[0].size(); two_pow_j *= 2, ++j) {
    for (size_t i = 0; i + two_pow_j <= buffer_.size(); i += 1) {

      bool flag = buffer_[table_[i][j - 1][0]] <
          buffer_[table_[i + two_pow_j / 2][j - 1][0]];
      const std::vector<int>& least_pair = flag ? table_[i][j - 1]
          : table_[i + two_pow_j / 2][j - 1];
      const std::vector<int>& largest_pair = !flag
          ? table_[i][j - 1] : table_[i + two_pow_j / 2][j - 1];

      if (j == 1) {
        table_[i][j][0] = least_pair[0];
        table_[i][j][1] = largest_pair[0];
        continue;
      }

      table_[i][j][0] = least_pair[0];
      table_[i][j][1] =  buffer_[least_pair[1]] < buffer_[largest_pair[0]]
          ? least_pair[1] : largest_pair[0];
    }
  }
}

void SparseTable::InitLogs() {
  logs_[1] = 0;
  size_t current_log = 0;
  size_t current_two_pow_i = 1;
  for (size_t i = 2; i < logs_.size(); ++i) {
    if (i == current_two_pow_i * 2) {
      current_log += 1;
      current_two_pow_i *= 2;
    }
    logs_[i] = current_log;
  }
}
NULL
int SparseTable::SecondStatistics(size_t left_bound, size_t right_bound) const {
  auto compare = [this](const int& lhvalue, const int& rhvalue) {
    return buffer_[lhvalue] < buffer_[rhvalue];
  };

  size_t j = logs_[right_bound - left_bound + 1];
  std::unordered_set<int> indexes = {table_[left_bound][j][0],
                                     table_[left_bound][j][1],
                                     table_[right_bound - (1 << j) + 1][j][0],
                                     table_[right_bound - (1 << j) + 1][j][1]};
  indexes.erase(std::min_element(indexes.begin(), indexes.end(), compare));

  return buffer_[*std::min_element(indexes.begin(), indexes.end(), compare)];
}
