// Задача 1_1 Хеш-функция.
// Реализуйте структуру данных типа “множество строк” на основе динамической
// хеш-таблицы с открытой адресацией. Хранимые строки непустые и состоят из
// строчных латинских букв. Хеш-функция строки должна быть реализована с
// помощью вычисления значения многочлена методом Горнера. Начальный размер
// таблицы должен быть равным 8-ми. Перехеширование выполняйте при добавлении
// элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.
// Структура данных должна поддерживать операции добавления строки в множество,
// удаления строки из множества и проверки принадлежности данной строки
// множеству. 1_1. Для разрешения коллизий используйте квадратичное
// пробирование. i-ая проба g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.
// 1_2. Для разрешения коллизий используйте двойное хеширование.
// Время работы O(n), Доп. память O(n)
#include <iostream>
#include <vector>
#include <string>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::max;
using std::move;

class HashTable {
 public:
  HashTable() = default;
  ~HashTable() = default;
  // Метод добавления элемента. Если элемент новый, возвращает true.
  bool Insert(const string &value);
  // Метод извлекает элемент. Если он присутствовал, возвращает true.
  bool Erase(const string &value);
  // Проверяет наличие элемента в таблице.
  bool Find(const string &value) const;
 private:

  class HashFunc {
   public:
    explicit HashFunc(size_t capacity);
    // Вычисляет значение хеш-функции для данной пробы.
    size_t operator()(const string &value, size_t probe_number = 0) const;
   private:
    static constexpr const double constant1_ = 0.5;
    static constexpr const double constant2_ = 0.5;
    unsigned multiplier_ = 347;
    size_t capacity_;
  };

  struct ElemType {
    string value = string();
    bool nil = true;
    bool deleted = false;
  };
  double load_factor_ = 0;
  const double max_load_factor_ = 0.75;
  size_t size_ = 0;
  size_t buffer_capacity_ = 8;
  vector<ElemType> buffer_ = vector<ElemType>(buffer_capacity_);
  HashFunc hash_func_ = HashFunc(buffer_capacity_);

  // В bool-flage возвращает нахождение элемента в таблице и индекс найденного
  // элемента, если элемент есть в таблице, либо индекс свободной ячейки.
  pair<bool, size_t> ElementSearch(const string &value) const;
  void Rehash();                            //Метод расширеня хеш-таблицы.
  inline bool CheckLoadFactor();
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  HashTable table;
  char command;
  string input;
  while (cin >> command >> input) {
    switch (command) {
      case '+':
        cout << (table.Insert(input) ? "OK" : "FAIL") << endl;
        break;
      case '-':
        cout << (table.Erase(input) ? "OK" : "FAIL") << endl;
        break;
      case '?':
        cout << (table.Find(input) ? "OK" : "FAIL") << endl;
        break;
      default:
        return 0;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

HashTable::HashFunc::HashFunc(size_t capacity)
    : capacity_(capacity)
      , multiplier_(347) {}


// Вычисление хеш-функции строки по методу Горнера с использование
// квадратичного пробирования.
size_t HashTable::HashFunc::operator()(const string &value,
                                       size_t probe_number) const {
  size_t hash_value = 0;
  for (auto symbol: value) {
    hash_value += symbol;
    hash_value *= multiplier_;
    hash_value %= capacity_;
  }
  return static_cast<size_t>((hash_value + constant1_ * probe_number
      + constant2_ * (probe_number * probe_number))) % capacity_;
}

bool HashTable::Insert(const string &value) {
  pair<bool, size_t> found = ElementSearch(value);
  if (found.first) {
    return false;
  } else {                                  // Нашли элемент -> вставляем его,
    buffer_[found.second] = {value, false, false};
    ++size_;                                // обновляем размер и проверяем на
    if (CheckLoadFactor()) {                // заполненность.
      Rehash();
    }
    return true;
  }
}

bool HashTable::Erase(const string &value) {
  pair<bool, size_t> found = ElementSearch(value);
  if (found.first) {
    buffer_[found.second].deleted = true;
    return true;
  } else {
    return false;
  }
}

bool HashTable::Find(const string &value) const {
  return ElementSearch(value).first;
}

pair<bool, size_t> HashTable::ElementSearch(const string &value) const {
  bool existing_flag = false;
  bool free_index_flag = false;
  size_t free_index;
  pair<bool, size_t> returned(false, 0);
  for (size_t probe_number = 0; probe_number < buffer_capacity_;
       ++probe_number) {
    size_t index = hash_func_(value, probe_number);
    if (buffer_[index].nil) {               // Дошли до конца кластера
      if ((!existing_flag) && (!free_index_flag)) {
        returned.second = index;
      }
      break;
    }
    if ((!buffer_[index].deleted) && (buffer_[index].value == value)) {
      existing_flag = true;                 // Нашли элемент с необходимым
      returned.second = index;              // значением.
      break;
    }
    if (buffer_[index].deleted) {           //Нашли удаленную ячейку
      if (!free_index_flag) {
        free_index_flag = true;
        free_index = index;
      }
    }
  }
  if ((!existing_flag) && (free_index_flag)) {
    returned.second = free_index;
  }
  returned.first = existing_flag;
  return returned;
}

void HashTable::Rehash() {
  buffer_capacity_ *= 2;
  vector<ElemType> new_buffer(buffer_capacity_);
  size_t new_size = 0;
  hash_func_ = HashFunc(buffer_capacity_);
  for (auto element: buffer_) {
    if ((!element.nil) && (!element.deleted)) { // Находим существующий элемент.
      for (size_t probe_number = 0; probe_number < buffer_capacity_;
           ++probe_number) {
        size_t index = hash_func_(element.value, probe_number);
        if (new_buffer[index].nil) {        // Попали в пустой элемент.
          ++new_size;
          new_buffer[index] = element;
          break;
        }
      }
    }
  }
  size_ = new_size;
  buffer_ = move(new_buffer);               // Копирование таблицы.
}

inline bool HashTable::CheckLoadFactor() {
  load_factor_ = static_cast<double>(size_) / buffer_capacity_;
  return (load_factor_ > max_load_factor_);
}