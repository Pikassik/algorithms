// Задача A. Восьминашки
// «Восьминашки» – упрощенный вариант известной головоломки «Пятнашки».
// Восемь костяшек, пронумерованных от 1 до 8 , расставлены по ячейкам
// игровой доски 3 на 3 , одна ячейка при этом остается пустой. За один ход
// разрешается передвинуть одну из костяшек, расположенных рядом с
// пустой ячейкой, на свободное место. Цель игры – для заданной
// начальной конфигурации игровой доски за минимальное число ходов
// получить выигрышную конфигурацию (пустая ячейка обозначена нулем):
// 1  2  3
// 4  5  6
// 7  8  0
// Время работы O(4^k), где k - число бога для восьминашек,
// равное 31. Доп. память O(4^k)

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <queue>

template<size_t Size>
class State {
 public:
  State() = default;
  explicit State(const std::vector<int>& configuration);
  explicit State(uint64_t state, size_t zero_index);

  // методы возвращают состояния при сдвиге пустой клетки в соотв. сторону
  State Up() const;
  State Down() const;
  State Left() const;
  State Right() const;
  std::vector<State> GetNextStates() const;

  // возвращает состояние в виде 64-битного числа
  uint64_t GetState() const;

  bool operator==(const State& rhvalue);
  bool operator!=(const State& rhvalue);
 private:
  // состояние в виде 64-битного числа
  uint64_t state_;
  size_t zero_index_;
  uint64_t multiply_by_mask(size_t index) const;
  uint64_t multiply_by_antimask(size_t index) const;
  static const std::vector<uint64_t> masks_;
  static const std::vector<uint64_t> antimasks_;
};

std::string GetPath(const std::vector<int>& configuration);

State<3> GenerateTarget();

std::string GeneratePath(const std::unordered_map<uint64_t, uint64_t>& parents,
                         const State<3>& source,
                         const State<3>& target);

bool IsPathExists(const std::vector<int>& configuration);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::vector<int> configuration;
  for (int i = 0; i < 9; ++i) {
    int number;
    std::cin >> number;
    configuration.push_back(number);
  }
  if (IsPathExists(configuration)) {
     std::string path = GetPath(configuration);
     std::cout << path.size() << std::endl;
     std::cout << path;
  } else {
    std::cout << -1 << std::endl;
  }
}

///////////////////////////////////////////////////////////////////////////////

template<size_t Size>
State<Size>::State(const std::vector<int>& configuration) {
  state_ = 0;
  for (size_t i = 0; i < Size * Size; ++i) {
    if (configuration[i] == 0) {
      zero_index_ = i;
    }

    state_ += configuration[i];

    if (i != Size * Size - 1) {
      state_ <<= 4;
    }
  }
}

template<size_t Size>
State<Size>::State(uint64_t state, size_t zero_index)
    :state_(state), zero_index_(zero_index) {}

template<size_t Size>
State<Size> State<Size>::Up() const {
  if (zero_index_ < Size) {
    return *this;
  }
  uint64_t new_zero_position_num = multiply_by_mask(zero_index_ - Size)
      >> 4 * Size;
  uint64_t new_state = multiply_by_antimask(zero_index_ - Size)
      + new_zero_position_num;
  return State(new_state, zero_index_ - Size);
}

template<size_t Size>
State<Size> State<Size>::Down() const {
  if (zero_index_ >= Size * Size - Size) {
    return *this;
  }

  uint64_t new_zero_position_num = multiply_by_mask(zero_index_ + Size)
      << 4 * Size;
  uint64_t new_state = multiply_by_antimask(zero_index_ + Size)
      + new_zero_position_num;
  return State(new_state, zero_index_ + Size);
}

template<size_t Size>
State<Size> State<Size>::Left() const {
  if (zero_index_ % Size == 0) {
    return *this;
  }

  uint64_t new_zero_position_num = multiply_by_mask(zero_index_ - 1) >> 4;
  uint64_t new_state = multiply_by_antimask(zero_index_ - 1)
      + new_zero_position_num;
  return State(new_state, zero_index_ - 1);
}

template<size_t Size>
class State<Size> State<Size>::Right() const {
  if (zero_index_ % Size == Size - 1) {
    return *this;
  }

  uint64_t new_zero_position_num = multiply_by_mask(zero_index_ + 1) << 4;
  uint64_t new_state = multiply_by_antimask(zero_index_ + 1)
      + new_zero_position_num;
  return State(new_state, zero_index_ + 1);
}

template<size_t Size>
uint64_t State<Size>::GetState() const {
  return state_;
}

template<size_t Size>
uint64_t State<Size>::multiply_by_mask(size_t index) const {
  size_t gap = (64 - Size * Size * 4);
  return ((state_ << gap) & masks_[index]) >> gap;
}

template<size_t Size>
uint64_t State<Size>::multiply_by_antimask(size_t index) const {
  size_t gap = (64 - Size * Size * 4);
  return ((state_ << gap) & antimasks_[index]) >> gap;

}

template<size_t Size>
bool State<Size>::operator==(const State& rhvalue) {
  return state_ == rhvalue.state_;
}

template<size_t Size>
bool State<Size>::operator!=(const State& rhvalue) {
  return state_ != rhvalue.state_;
}

template<size_t Size>
std::vector<class State<Size>> State<Size>::GetNextStates() const {
  return {Up(), Down(), Left(), Right()};
}

template<size_t Size>
const std::vector<uint64_t> State<Size>::State::masks_ = {
    17293822569102704640ull, //11110000........
    1080863910568919040ull,  //00001111........
    67553994410557440ull,    //000000001111....
    4222124650659840ull,     //................
    263882790666240ull,
    16492674416640ull,
    1030792151040ull,
    64424509440ull,
    4026531840ull,
    251658240ull,
    15728640ull,
    983040ull,
    61440ull,
    3840ull,
    240ull,
    15ull                    //........00001111
};

template<size_t Size>
const std::vector<uint64_t> State<Size>::State::antimasks_ = {
    1152921504606846975ull,  //00001111........
    17365880163140632575ull, //111100001111....
    18379190079298994175ull, //111111110000....
    18442521949058891775ull, //................
    18446480190918885375ull,
    18446727581035134975ull,
    18446743042917400575ull,
    18446744009285042175ull,
    18446744069683019775ull,
    18446744073457893375ull,
    18446744073693822975ull,
    18446744073708568575ull,
    18446744073709490175ull,
    18446744073709547775ull,
    18446744073709551375ull,
    18446744073709551600ull  //........11110000
};

///////////////////////////////////////////////////////////////////////////////

// Алгоритм представляет обычный BFS.
std::string GetPath(const std::vector<int>& configuration) {
  State<3> source = State<3>(configuration);
  State<3> target = GenerateTarget();

  // хеш-таблица, для хранения перемещений, которыми получено состояние.
  // 0 - Up, 1 - Down, 2 - Left, 3 - Right
  std::unordered_map<uint64_t, uint64_t> parents;
  std::queue<State<3>> queue;
  queue.emplace(source);
  bool flag = false;
  while (!queue.empty()) {
    if (flag) {
      break;
    }

    std::vector<State<3>> next = queue.front().GetNextStates();
    for (size_t i = 0; i < next.size(); ++i) {
      if (parents.find(next[i].GetState()) == parents.end()) {
        parents[next[i].GetState()] = i;

        if (next[i] == target) {
          flag = true;
          break;
        }

        queue.push(next[i]);
      }
    }

    queue.pop();
  }

  return GeneratePath(parents, source, target);
}

class State<3> GenerateTarget() {
  uint64_t target = 0;
  for (uint64_t i = 1; i < 9; ++i) {
    target += i;
    target <<= 4;
  }
  return State<3>(target, 8);
}

std::string GeneratePath(const std::unordered_map<uint64_t, uint64_t>& parents,
                         const State<3>& source,
                         const State<3>& target) {
  //0 up
  //1 down
  //2 left
  //3 right
  std::string path;
  State<3> current = target;
  while (current != source) {
    if (parents.at(current.GetState()) == 0) {
      path.push_back('U');
      current = current.Down();
    } else if (parents.at(current.GetState()) == 1) {
      path.push_back('D');
      current = current.Up();
    } else if (parents.at(current.GetState()) == 2) {
      path.push_back('L');
      current = current.Right();
    } else if (parents.at(current.GetState()) == 3) {
      path.push_back('R');
      current = current.Left();
    }
  }
  std::reverse(path.begin(), path.end());
  return path;
}

// Подсчитывает количество инверсий и проверяет ее четность,
// что является критерием наличия пути сборки.
bool IsPathExists(const std::vector<int>& configuration) {
  int inversion_count = 0;
  for (int i = 0; i < configuration.size(); ++i) {
    for (int j = i + 1; j < configuration.size(); ++j) {
      if (configuration[j] == 0 || configuration[i] == 0) {
        continue;
      }
      if (configuration[i] > configuration[j]) {
        ++inversion_count;
      }
    }
  }
  return inversion_count % 2 != 1;
}
