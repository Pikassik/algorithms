#include <iostream>
#include <algorithm>
#include <array>
#include <queue>

class MinMovesToCheckmateCalculater {
 public:
  struct State {
    int king_x;
    int king_y;
    int queen_x;
    int queen_y;
    bool is_white;
  };
  MinMovesToCheckmateCalculater(const State& beginning_state);
  static constexpr int kInfinity = INT32_MAX / 2;
  int Calculate();
 private:
  static const std::array<State, 19> win_states_;
  static const std::array<std::pair<int, int>, 8> white_king_threat_;
  static bool IsCorrect(int x, int y) noexcept;
  static bool WhiteKingThreat(int x, int y) noexcept;
  static bool BlackKingThreat(const State& state) noexcept;
  static bool QueenThreat(const State& state) noexcept;
  static bool KingQueenMatch(const State& state) noexcept;
  static std::array<State, 56>
  GetAllQueenMoves(const State& state, bool flag) noexcept;
  static std::array<State, 8>
  GetAllKingMoves(const State& state, bool flag) noexcept;
  void AddReverseKingMoves(const State& state);
  void AddReverseQueenMoves(const State& state);
  void InitDistances() noexcept;
  int& GetDistance(const State& state) noexcept;
  void ProcessWhiteState(const State& current_state) noexcept;
  void ProcessBlackState(const State& current_state) noexcept;
  std::array<std::array<std::array<std::array<std::array<int, 2>, 8>, 8>, 8>, 8>
  distances_;
  std::queue<State> states_queue_;
  State beginning_state_;
};

namespace {
  typedef MinMovesToCheckmateCalculater::State _State;
}

////////////////////////////////////////////////////////////////////////////////
int main() {
  _State beginning_state;
  std::string input;
  getline(std::cin, input);
  char begin_king_x = input[3],
       begin_king_y = input[4],
       begin_queen_x = input[0],
       begin_queen_y = input[1];
  beginning_state = {begin_king_x  - 'a',
                     begin_king_y  - '1',
                     begin_queen_x - 'a',
                     begin_queen_y - '1',
                     true};
  MinMovesToCheckmateCalculater calculater(beginning_state);
  std::cout << calculater.Calculate() << std::endl;
}
///////////////////////////////////////////////////////////////////////////////

bool MinMovesToCheckmateCalculater::IsCorrect(int x, int y) noexcept {
  return  x <= 7 && x >= 0 && y <= 7 && y >= 0;
}

MinMovesToCheckmateCalculater::
MinMovesToCheckmateCalculater(const _State& beginning_state)
: beginning_state_(beginning_state) {
  InitDistances();
  for (const auto& win: win_states_) {
    GetDistance(win) = 0;
    AddReverseQueenMoves(win);
  }
}

bool MinMovesToCheckmateCalculater::WhiteKingThreat(int x, int y) noexcept {
  return std::find(white_king_threat_.begin(),
                   white_king_threat_.end(),
                   std::make_pair(x, y)) !=
         white_king_threat_.end();
}

bool MinMovesToCheckmateCalculater::
BlackKingThreat(const _State& state) noexcept {
  return ( std::abs(state.king_y - state.queen_y) == 1 &&
          (std::abs(state.king_x - state.queen_x) == 1 ||
          state.king_x == state.queen_x)) ||

          (state.king_y == state.queen_y &&
          std::abs(state.king_x - state.queen_x) == 1);
}

bool MinMovesToCheckmateCalculater::
QueenThreat(const _State& state) noexcept {
  auto q_movs = GetAllQueenMoves(state, true);
  for (int i = 0; i < q_movs.size(); ++i) {
    State next = q_movs[i];
    if (!IsCorrect(next.queen_x, next.queen_y))
      continue;

    if (next.queen_x == 2 && next.queen_y == 2) {
      i = ((i / 7) + 1) * 7 - 1;
      continue;
    }

    if (next.queen_x == next.king_x && next.queen_y == next.king_y)
      return true;
  }

  return false;
}

bool MinMovesToCheckmateCalculater::
KingQueenMatch(const _State& state) noexcept {
  return state.king_x == state.queen_x &&
         state.king_y == state.queen_y;
}

std::array<_State, 56> MinMovesToCheckmateCalculater::
GetAllQueenMoves(const _State& state, bool flag) noexcept {
  return {{
    {state.king_x, state.king_y, state.queen_x + 1, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 2, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 3, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 4, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 5, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 6, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x + 7, state.queen_y    , flag},

    {state.king_x, state.king_y, state.queen_x - 1, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 2, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 3, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 4, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 5, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 6, state.queen_y    , flag},
    {state.king_x, state.king_y, state.queen_x - 7, state.queen_y    , flag},

    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 1, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 2, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 3, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 4, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 5, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 6, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y + 7, flag},

    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 1, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 2, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 3, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 4, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 5, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 6, flag},
    {state.king_x, state.king_y, state.queen_x    , state.queen_y - 7, flag},

    {state.king_x, state.king_y, state.queen_x + 1, state.queen_y + 1, flag},
    {state.king_x, state.king_y, state.queen_x + 2, state.queen_y + 2, flag},
    {state.king_x, state.king_y, state.queen_x + 3, state.queen_y + 3, flag},
    {state.king_x, state.king_y, state.queen_x + 4, state.queen_y + 4, flag},
    {state.king_x, state.king_y, state.queen_x + 5, state.queen_y + 5, flag},
    {state.king_x, state.king_y, state.queen_x + 6, state.queen_y + 6, flag},
    {state.king_x, state.king_y, state.queen_x + 7, state.queen_y + 7, flag},

    {state.king_x, state.king_y, state.queen_x - 1, state.queen_y - 1, flag},
    {state.king_x, state.king_y, state.queen_x - 2, state.queen_y - 2, flag},
    {state.king_x, state.king_y, state.queen_x - 3, state.queen_y - 3, flag},
    {state.king_x, state.king_y, state.queen_x - 4, state.queen_y - 4, flag},
    {state.king_x, state.king_y, state.queen_x - 5, state.queen_y - 5, flag},
    {state.king_x, state.king_y, state.queen_x - 6, state.queen_y - 6, flag},
    {state.king_x, state.king_y, state.queen_x - 7, state.queen_y - 7, flag},

    {state.king_x, state.king_y, state.queen_x + 1, state.queen_y - 1, flag},
    {state.king_x, state.king_y, state.queen_x + 2, state.queen_y - 2, flag},
    {state.king_x, state.king_y, state.queen_x + 3, state.queen_y - 3, flag},
    {state.king_x, state.king_y, state.queen_x + 4, state.queen_y - 4, flag},
    {state.king_x, state.king_y, state.queen_x + 5, state.queen_y - 5, flag},
    {state.king_x, state.king_y, state.queen_x + 6, state.queen_y - 6, flag},
    {state.king_x, state.king_y, state.queen_x + 7, state.queen_y - 7, flag},

    {state.king_x, state.king_y, state.queen_x - 1, state.queen_y + 1, flag},
    {state.king_x, state.king_y, state.queen_x - 2, state.queen_y + 2, flag},
    {state.king_x, state.king_y, state.queen_x - 3, state.queen_y + 3, flag},
    {state.king_x, state.king_y, state.queen_x - 4, state.queen_y + 4, flag},
    {state.king_x, state.king_y, state.queen_x - 5, state.queen_y + 5, flag},
    {state.king_x, state.king_y, state.queen_x - 6, state.queen_y + 6, flag},
    {state.king_x, state.king_y, state.queen_x - 7, state.queen_y + 7, flag}
  }};
}

std::array<_State, 8> MinMovesToCheckmateCalculater::
GetAllKingMoves(const _State& state, bool flag) noexcept {
  return  {{
      {state.king_x + 1, state.king_y,     state.queen_x, state.queen_y, flag},
      {state.king_x - 1, state.king_y,     state.queen_x, state.queen_y, flag},
      {state.king_x,     state.king_y + 1, state.queen_x, state.queen_y, flag},
      {state.king_x,     state.king_y - 1, state.queen_x, state.queen_y, flag},
      {state.king_x + 1, state.king_y + 1, state.queen_x, state.queen_y, flag},
      {state.king_x - 1, state.king_y - 1, state.queen_x, state.queen_y, flag},
      {state.king_x + 1, state.king_y - 1, state.queen_x, state.queen_y, flag},
      {state.king_x - 1, state.king_y + 1, state.queen_x, state.queen_y, flag}
  }};
}

void MinMovesToCheckmateCalculater::
AddReverseKingMoves(const _State& state) {
  for (auto next: GetAllKingMoves(state, false)) {
    if (!(!IsCorrect(next.king_x, next.king_y) ||
           KingQueenMatch(next) ||
          (next.king_x == 2 && next.king_y == 2) ||
           WhiteKingThreat(next.king_x, next.king_y) ||
          (GetDistance(next) != kInfinity &&
           GetDistance(next) >= GetDistance(state) + 1)))
      states_queue_.push(next);
  }
}

void MinMovesToCheckmateCalculater::
AddReverseQueenMoves(const _State& state) {
  auto queen_movs = GetAllQueenMoves(state, true);
  for (int i = 0; i < queen_movs.size(); ++i) {
    State next = queen_movs[i];
    if ((next.queen_x == 2 && next.queen_y == 2) ||
        (next.queen_x == next.king_x && next.queen_y == next.king_y)) {
      i = ((i / 7) + 1) * 7 - 1;
      continue;
    }

    if (!(!IsCorrect(next.queen_x, next.queen_y) ||
          QueenThreat(next) ||
          WhiteKingThreat(next.king_x, next.king_y) ||
          (GetDistance(next) != kInfinity &&
          GetDistance(next) <= GetDistance(state) + 1)))
      states_queue_.push(next);
  }
}

void MinMovesToCheckmateCalculater::InitDistances() noexcept {
  for (int king_x = 0; king_x < 8; ++king_x) {
    for (int king_y = 0; king_y < 8; ++king_y) {
      for (int queen_x = 0; queen_x < 8; ++queen_x) {
        for (int queen_y = 0; queen_y < 8; ++queen_y) {
          distances_[king_x][king_y][queen_x][queen_y][true] = kInfinity;
          distances_[king_x][king_y][queen_x][queen_y][false] = kInfinity;
        }
      }
    }
  }
}

int& MinMovesToCheckmateCalculater::GetDistance(const _State& state) noexcept {
  return distances_[state.king_x]
                   [state.king_y]
                   [state.queen_x]
                   [state.queen_y]
                   [state.is_white];
}

const std::array<_State, 19> MinMovesToCheckmateCalculater::win_states_ = {{
// king_x king_x queen_x queen_y  is_w
    { 0,  0,  1,  1, false}, // 1
    { 0,  1,  1,  1, false}, // 2
    { 1,  0,  1,  1, false}, // 3

    { 2,  0,  2,  1, false}, // 4
    { 3,  0,  3,  1, false}, // 5

    { 0,  2,  1,  2, false}, // 6
    { 0,  3,  1,  3, false}, // 7

    { 2,  0,  4,  0, false}, // 8
    { 2,  0,  5,  0, false}, // 9
    { 2,  0,  6,  0, false}, // 10
    { 2,  0,  7,  0, false}, // 11

    { 0,  2,  0,  4, false}, // 12
    { 0,  2,  0,  5, false}, // 13
    { 0,  2,  0,  6, false}, // 14
    { 0,  2,  0,  7, false}, // 15
    { 3,  0,  5,  0, false}, // 16
    { 0,  3,  0,  5, false}, // 17

    { 2,  0,  0,  0, false}, // 18
    { 0,  2,  0,  0, false}  // 19
}};

const std::array<std::pair<int, int>, 8>
MinMovesToCheckmateCalculater::white_king_threat_ = {{
    {1, 1},
    {2, 1},
    {3, 1},
    {3, 2},
    {3, 3},
    {2, 3},
    {1, 3},
    {1, 2}
}};

int MinMovesToCheckmateCalculater::Calculate() {
  while (!states_queue_.empty() &&
      GetDistance(beginning_state_) == kInfinity) {
    State current_state = states_queue_.front();
    states_queue_.pop();
    if (current_state.is_white) {
      ProcessWhiteState(current_state);
    } else {
      ProcessBlackState(current_state);
    }
  }

  return GetDistance(beginning_state_);
}

void MinMovesToCheckmateCalculater::
ProcessWhiteState(const _State& current_state) noexcept {
  int min_distance = kInfinity;
  if (QueenThreat(current_state) ||
      WhiteKingThreat(current_state.king_x, current_state.king_y))
    return;

  auto queen_moves = GetAllQueenMoves(current_state, false);
  for (size_t i = 0; i < queen_moves.size(); ++i) {
    State next = queen_moves[i];
    if ((next.queen_x == 2 && next.queen_y == 2) ||
        (next.queen_x == next.king_x && next.queen_y == next.king_y)) {
      i = ((i / 7) + 1) * 7 - 1;
      continue;
    }
    if (!(!IsCorrect(next.queen_x, next.queen_y) ||
          (BlackKingThreat(next) &&
           !WhiteKingThreat(next.queen_x, next.queen_y)))) {
      min_distance = std::min(min_distance, GetDistance(next));
    }
  }
  if (GetDistance(current_state) > min_distance + 1) {
    GetDistance(current_state) = min_distance + 1;
    AddReverseKingMoves(current_state);
  }
}

void MinMovesToCheckmateCalculater::
ProcessBlackState(const _State& current_state) noexcept {
  int max_distance = -kInfinity;
  for (const auto& next: GetAllKingMoves(current_state, true)) {
    if (!(!IsCorrect(next.king_x, next.king_y)   ||
          (next.king_x == 2 && next.king_y == 2) ||
          KingQueenMatch(next) ||
          QueenThreat(next) ||
          WhiteKingThreat(next.king_x, next.king_y)))
      max_distance = std::max(max_distance, GetDistance(next));
  }

  if (max_distance != kInfinity &&
     (GetDistance(current_state) == kInfinity ||
      GetDistance(current_state) < max_distance + 1)) {
    GetDistance(current_state) = max_distance + 1;
    AddReverseQueenMoves(current_state);
  }
}
