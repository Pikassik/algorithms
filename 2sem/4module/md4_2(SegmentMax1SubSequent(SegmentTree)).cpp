// Дан массив из нулей и единиц a0, a1, ..., an-1.
// Для каждого запроса [left, right] найдите такой
// подотрезок al, al+1, ..., ar этого массива
// (0 <= left <= l <= r <= right < n), что
// числа al, al+1, …, ar являются максимально
// возможной последовательностью единиц.
// Требуемое время ответа на запрос - O(log n).
// Доп. память - O(n), Время построения - O(n)

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

class SequenceSegmentTree {
 public:
  explicit SequenceSegmentTree(const std::vector<int>& sequence);
  int MaxSubsequence(size_t left_bound, size_t right_bound);
 private:
  struct Segment {
    int size;
    int largest_subword;
    int largest_prefix;
    int largest_suffix;
  };
  Segment Query(size_t segment,
                size_t segment_left_bound,
                size_t segment_right_bound,
                size_t left_bound,
                size_t right_bound) const;
  void InitTree(const std::vector<int>& sequence);
  std::vector<Segment> segment_tree_;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  int sequence_size, requests_count;
  std::cin >> sequence_size >> requests_count;
  std::vector<int> sequence;
  for (int i = 0; i < sequence_size; ++i) {
    int input;
    std::cin >> input;
    sequence.push_back(input);
  }
  SequenceSegmentTree tree(sequence);

  for (int i = 0; i < requests_count; ++i) {
    size_t left_bound, right_bound;
    std::cin >> left_bound >> right_bound;
    std::cout << tree.MaxSubsequence(left_bound, right_bound) << std::endl;
  }
}

///////////////////////////////////////////////////////////////////////////////

SequenceSegmentTree::SequenceSegmentTree(const std::vector<int>& sequence)
    : segment_tree_(
    (1 << (static_cast<int>(std::ceil(std::log2(sequence.size()))) + 1)) - 1,
    {1, 0, 0, 0}) {
  InitTree(sequence);
}

void SequenceSegmentTree::InitTree(const std::vector<int>& sequence) {
  for (size_t i = segment_tree_.size() / 2, j = 0; j < sequence.size();
       ++i, ++j) {
    if (sequence[j] == 1) {
      segment_tree_[i] = {1, 1, 1, 1};
    }
  }

  for (int i = segment_tree_.size() / 2 - 1; i >= 0; --i) {
    segment_tree_[i].size = 2 * segment_tree_[2 * i + 1].size;

    segment_tree_[i].largest_subword = std::max(
        {segment_tree_[2 * i + 1].largest_subword,
         segment_tree_[2 * i + 2].largest_subword,
         segment_tree_[2 * i + 1].largest_suffix +
             segment_tree_[2 * i + 2].largest_prefix}
    );

    segment_tree_[i].largest_prefix =
        segment_tree_[2 * i + 1].largest_prefix == segment_tree_[2 * i + 1].size
        ? segment_tree_[2 * i + 1].size +
            segment_tree_[2 * i + 2].largest_prefix
        : segment_tree_[2 * i + 1].largest_prefix;

    segment_tree_[i].largest_suffix =
        segment_tree_[2 * i + 2].largest_suffix == segment_tree_[2 * i + 2].size
        ? segment_tree_[2 * i + 2].size +
            segment_tree_[2 * i + 1].largest_suffix
        : segment_tree_[2 * i + 2].largest_suffix;
  }
}

int SequenceSegmentTree::MaxSubsequence(size_t left_bound, size_t right_bound) {
  return Query(0, 0, segment_tree_.size() / 2, left_bound, right_bound).
      largest_subword;
}

SequenceSegmentTree::Segment SequenceSegmentTree::Query(size_t segment,
                               size_t segment_left_bound,
                               size_t segment_right_bound,
                               size_t left_bound,
                               size_t right_bound) const {
  if (segment_left_bound > right_bound || segment_right_bound < left_bound) {
    return {static_cast<int>(segment_right_bound - segment_left_bound + 1),
            0, 0, 0};
  }

  if (segment_left_bound >= left_bound && segment_right_bound <= right_bound) {
    return {static_cast<int>(segment_right_bound - segment_left_bound + 1),
            segment_tree_[segment].largest_subword,
            segment_tree_[segment].largest_prefix,
            segment_tree_[segment].largest_suffix};
  }

  size_t middle_bound = (segment_left_bound + segment_right_bound) / 2;

  auto left_segment = Query(2 * segment + 1, segment_left_bound, middle_bound,
               left_bound, right_bound);
  auto right_segment = Query(2 * segment  + 2, middle_bound + 1, segment_right_bound,
                    left_bound, right_bound);

  return {left_segment.size + right_segment.size,
          std::max(
              {left_segment.largest_subword,
               right_segment.largest_subword,
               left_segment.largest_suffix + right_segment.largest_prefix}
          ),
          left_segment.largest_prefix == left_segment.size
          ? left_segment.size + right_segment.largest_prefix
          : left_segment.largest_prefix,
          right_segment.largest_suffix == right_segment.size
          ? right_segment.size + left_segment.largest_suffix
          : right_segment.largest_suffix
          };
}
