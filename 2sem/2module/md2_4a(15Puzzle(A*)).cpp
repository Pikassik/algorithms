// D1. Пятнашки (A)
// Написать алгоритм для решения игры в “пятнашки”.
// Решением задачи является приведение к виду:
// [ 1 2 3 4 ] [ 5 6 7 8 ] [ 9 10 11 12] [ 13 14 15 0 ],
// где 0 задает пустую ячейку. Достаточно найти хотя бы
// какое-то решение. Число перемещений костяшек не обязано
// быть минимальным.
// Время работы Omega(k), O(e^n), где k - число бога для пятнашек,
// равное 80, n - размер таблицы, равный 4
// Доп. память O(e^n)

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <cmath>
#include <set>

template<size_t Size>
class State {
 public:
  State() = default;
  explicit State(const std::vector<int>& configuration);
  explicit State(uint64_t state);
  explicit State(uint64_t state, size_t zero_index);
  
  // методы возвращают состояния при сдвиге пустой клетки в соотв. сторону 
  State Up() const;
  State Down() const;
  State Left() const;
  State Right() const;
  std::vector<State> GetNextStates() const;
  
  // возвращает состояние в виде 64-битного числа
  uint64_t GetState() const;
  
  // возвращает число на i-й позиции таблицы
  int operator[](size_t index) const;
  
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

int Heuristic(const State<4>& state);

State<4> GenerateTarget();

std::string GeneratePath(const std::unordered_map<uint64_t, uint64_t>& parents,
                         const State<4>& source,
                         const State<4>& target);

bool IsPathExists(const std::vector<int>& configuration);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::vector<int> configuration;
  for (int i = 0; i < 16; ++i) {
    int number;
    std::cin >> number;
    configuration.push_back(number);
  }
  
  if (IsPathExists(configuration)) {
    std::string path = GetPath(configuration);
    std::cout << path.size() << std::endl;
    std::cout << path << std::endl;
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
State<Size>::State(uint64_t state) :state_(state) {
  for (size_t i = 0; i < Size * Size; ++i) {
    if ((*this)[i] == 0) {
      zero_index_ = i;
      break;
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

// Работает ли?
template<size_t Size>
int State<Size>::operator[](size_t index) const {
  return multiply_by_mask(index) >> ((Size * Size - 1 - index) * 4);
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


std::string GetPath(const std::vector<int>& configuration) {
  State<4> source = State<4>(configuration);
  State<4> target = GenerateTarget();

  // первый элемент - эвристика, второй - состояние доски
  std::set<std::pair<uint64_t, uint64_t>> priority_queue;

  // хеш-таблица движений, при которых получено состояние
  std::unordered_map<uint64_t, uint64_t> parents;

  priority_queue.emplace(Heuristic(source), source.GetState());

  while ((*priority_queue.begin()).second != target.GetState()) {
    auto vertex = *priority_queue.begin();
    std::vector<State<4>> next_states =
                                      (State<4>(vertex.second)).GetNextStates();

    //0 - Up, 1 - Down, 2 - Left, 3 - Right
    for (size_t i = 0; i < next_states.size(); ++i) {

      // Состояние не посещено
      if (parents.find(next_states[i].GetState()) == parents.end()) {
        parents[next_states[i].GetState()] = i;
        priority_queue.emplace(Heuristic(next_states[i]), next_states[i].GetState());
      }
    }
    priority_queue.erase(vertex);
  }

  return GeneratePath(parents, source, target);
}

State<4> GenerateTarget() {
  uint64_t target = 0;
  for (uint64_t i = 1; i < 16; ++i) {
    target += i;
    target <<= 4;
  }

  return State<4>(target, 15);
}

// Возвращаемся обратно к истоку
std::string GeneratePath(const std::unordered_map<uint64_t, uint64_t>& parents,
                         const State<4>& source,
                         const State<4>& target) {
  //0 up
  //1 down
  //2 left
  //3 right
  std::string path;
  State<4> current = target;
  while (current != source) {
    if (parents.at(current.GetState()) == 0) {
      path.push_back('D');
      current = current.Down();
    } else if (parents.at(current.GetState()) == 1) {
      path.push_back('U');
      current = current.Up();
    } else if (parents.at(current.GetState()) == 2) {
      path.push_back('R');
      current = current.Right();
    } else if (parents.at(current.GetState()) == 3) {
      path.push_back('L');
      current = current.Left();
    }
  }
  std::reverse(path.begin(), path.end());
  return path;
}

// Подсчет инверсий в развороте состояния "змейкой"
bool IsPathExists(const std::vector<int>& configuration) {
  int inversion_count = 0;
  std::vector<int> copied_configuration = configuration;
  std::swap(copied_configuration[4], copied_configuration[7]);
  std::swap(copied_configuration[5], copied_configuration[6]);
  std::swap(copied_configuration[12], copied_configuration[15]);
  std::swap(copied_configuration[13], copied_configuration[14]);

  for (int i = 0; i < copied_configuration.size(); ++i) {
    for (int j = i + 1; j < copied_configuration.size(); ++j) {
      if (copied_configuration[j] == 0 || copied_configuration[i] == 0) {
        continue;
      }
      if (copied_configuration[i] > copied_configuration[j]) {
        ++inversion_count;
      }
    }
  }

  return inversion_count % 2 == 1;
}

// Эвристика, заключающаяся в подсчете сумм
// манхэттэнских расстояний от каждого
// элемента до его места в собранном виде.
int Heuristic(const State<4>& state) {
  int heuristic = 0;
  for (int i = 0; i < 16; ++i) {
    int x_coord = i % 4;
    int y_coord = i / 4;
    int target_x_coord = state[i] == 0 ? 3 : (state[i] - 1) % 4;
    int target_y_coord = state[i] == 0 ? 3 : (state[i] - 1) / 4;
    heuristic += abs(x_coord - target_x_coord) + abs(y_coord - target_y_coord);
  }
  return heuristic;
}
