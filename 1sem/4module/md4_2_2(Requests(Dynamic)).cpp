// 2_2. Заявки на переговоры.
// В большой IT-фирме есть только одна переговорная комната. Желающие
// посовещаться заполняют заявки с желаемым временем начала и конца.
// Ваша задача определить максимальное количество заявок, которое может быть
// удовлетворено. Число заявок ≤ 100000.
// Время работы O(n * log(n)), Доп. память O(n).
#include <iostream>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::sort;
using std::make_pair;

class RequestsCompare {
 public:
  // Компаратор сравнивает запросы по правому концу (времени окончания).
  bool operator()(pair<int, int> left, pair<int, int> right);
};

size_t RequestsCount(vector<pair<int, int>> &requests);

///////////////////////////////////////////////////////////////////////////////

int main() {
  vector<pair<int, int>> requests;
  int begin, end;
  while (cin >> begin >> end) {
    requests.emplace_back(begin, end);
  }
  cout << RequestsCount(requests);
  return 0;
}

bool RequestsCompare::operator()(pair<int, int> left, pair<int, int> right) {
  return left.second < right.second;
}

size_t RequestsCount(vector<pair<int, int>> &requests) {
  RequestsCompare cmp;
  sort(requests.begin(), requests.end(), cmp);
  size_t counter = 1;
  pair<int, int> current = requests[0];
  // Проходимся по массиву запросов.
  for (size_t i = 1; i < requests.size(); ++i) {
    // Как только находим запрос, начало которого больше равно начала
    // текущего, обновляем счетчик и переходим к нему.
    if (current.second <= requests[i].first) {
      current = requests[i];
      ++counter;
    }
  }
  return counter;
}