// Задача 3. ДП 3_1. Высокая пирамида.
// Дано N кубиков. Требуется определить каким количеством способов можно
// выстроить из этих кубиков пирамиду. Каждый вышележащий слой пирамиды должен
// быть не больше нижележащего. Время работы O(n^2). Доп. память O(n^2).

#include <iostream>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

class CubesPermutations {
 public:
  CubesPermutations() = default;
  ~CubesPermutations() = default;
  long long GetPermutations(unsigned count);//Метод, запускающий рекурсию и
 private:                                   //первоначально заполняющий таблицу.
  long long GetCount(int i, int j);         //Рекурсивная функция.
  vector<vector<long long>> buffer_;        //Таблица для ДП.
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  unsigned count;
  cin >> count;
  CubesPermutations permutations;
  cout << permutations.GetPermutations(count) << endl;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

long long CubesPermutations::GetPermutations(unsigned count) {
  buffer_ =                                 //Выделение памяти для таблицы NxN+1
      vector<vector<long long>>(count + 1, vector<long long>(count + 1, 0));
  for (int i = 1; i <= count; ++i) {        //Заполнение 0-го столбца единицами
    buffer_[i][0] = 1;                      //(пустые пирамидки)
  }
  for (int i = 1; i <= count; ++i) {        //Заполнение 1-го столбца единицами
    buffer_[i][1] = 1;
  }
  for (int j = 1; j <= count; ++j) {        //Заполнение 1-й строки единицами
    buffer_[1][j] = 1;
  }
  return GetCount(count, count);
}

long long CubesPermutations::GetCount(int i, int j) {
  if ((i < 1) || (j < 0)) {                 //При выходе за границу таблицы
    return 0;                               //происходит окончание рекурсии
  }
  if (buffer_[i][j] == 0) {                 //Если попали в незаполненную ячейку
    buffer_[i][j] = GetCount(i - 1, j) + GetCount(i, j - i);
  }                                         //то рекурсивно заполняем ее
  return buffer_[i][j];
}