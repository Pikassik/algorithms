/*
 * Дана строка длины n. Найти количество ее различных подстрок.
 * Используйте суффиксный массив.
 * Построение суффиксного массива выполняйте за O(n log n).
 * Вычисление количества различных подстрок выполняйте за O(n).
 */

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

const size_t kCharsCount = UINT8_MAX + 1;

std::vector<size_t>
SuffixArray(const std::string& input_string);

void
InitSubSuffixArray(const std::string& string,
                   std::vector<size_t>& sub_suffix_array,
                   std::vector<size_t>& classes,
                   size_t& classes_count);

std::vector<size_t>
BuildSuffixArrayAfterInit(const std::string& string,
                          std::vector<size_t>& sub_suffix_array,
                          std::vector<size_t>& classes,
                          size_t& classes_count);

std::vector<size_t>
LongestCommonPrefixArray(const std::string& string,
                         const std::vector<size_t>& suffix_array);

void
ShiftPositionsInSuffixArray(const std::string& string,
                            const std::vector<size_t>& sub_suffix_array,
                            std::vector<size_t>& new_sub_suffix_array,
                            size_t k_to_power_of_two);

void
SubCountingSortViaClasses(const std::string& string,
                          std::vector<size_t>& sub_suffix_array,
                          const std::vector<size_t>& new_sub_suffix_array,
                          const std::vector<size_t>& classes,
                          std::vector<size_t>& classes_counter,
                          size_t classes_count);

void
UpdateClasses(const std::string& string,
              const std::vector<size_t>& sub_suffix_array,
              std::vector<size_t>& classes,
              std::vector<size_t>& new_classes,
              size_t& classes_count,
              size_t k_to_power_of_two);

size_t
CalculateDifferentSubstringsCount(const std::string& string);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::string string;
  std::cin >> string;
  std::cout << CalculateDifferentSubstringsCount(string);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<size_t>
SuffixArray(const std::string& input_string) {
  std::string string = input_string + '\0';
  std::vector<size_t> sub_suffix_array(string.size());
  std::vector<size_t> classes(string.size());
  size_t classes_count = 1;
  InitSubSuffixArray(string, sub_suffix_array, classes, classes_count);
  return
    BuildSuffixArrayAfterInit(string, sub_suffix_array, classes, classes_count);
}

void
InitSubSuffixArray(const std::string& string,
                   std::vector<size_t>& sub_suffix_array,
                   std::vector<size_t>& classes,
                   size_t& classes_count) {
  std::vector<size_t> char_count(kCharsCount, 0);
  for (size_t i = 0; i < string.size(); ++i) {
    ++char_count[string[i]];
  }

  for (size_t i = 0; i < kCharsCount - 1; ++i) {
    char_count[i + 1] += char_count[i];
  }

  for (size_t i = 0; i < string.size(); ++i) {
    --char_count[string[i]];
    sub_suffix_array[char_count[string[i]]] = i;
  }
  char_count.clear();

  classes_count = 1;
  classes[0] = 0;
  for (size_t i = 0; i < string.size() - 1; ++i) {
    if (string[sub_suffix_array[i + 1]] !=
        string[sub_suffix_array[i]]) {
      ++classes_count;
    }
    classes[sub_suffix_array[i + 1]] = classes_count - 1;
  }
}

std::vector<size_t>
BuildSuffixArrayAfterInit(const std::string& string,
                          std::vector<size_t>& sub_suffix_array,
                          std::vector<size_t>& classes,
                          size_t& classes_count) {
  auto power_of_two = [](size_t power) {
    return static_cast<size_t>(1) << power;
  };

  std::vector<size_t> classes_counter(string.size());
  std::vector<size_t> new_sub_suffix_array(string.size());
  std::vector<size_t> new_classes(string.size());

  for (size_t k = 0, k_to_power_of_two = power_of_two(k);
       k_to_power_of_two < string.size();
       ++k, k_to_power_of_two = power_of_two(k)) {
    ShiftPositionsInSuffixArray(string,
                                sub_suffix_array,
                                new_sub_suffix_array,
                                k_to_power_of_two);

    SubCountingSortViaClasses(string,
                              sub_suffix_array,
                              new_sub_suffix_array,
                              classes,
                              classes_counter,
                              classes_count);

    UpdateClasses(string,
                  sub_suffix_array,
                  classes,
                  new_classes,
                  classes_count,
                  k_to_power_of_two);
  }

  std::rotate(sub_suffix_array.begin(),
              sub_suffix_array.begin() + 1,
              sub_suffix_array.end());
  sub_suffix_array.pop_back();
  return std::move(sub_suffix_array);
}

void ShiftPositionsInSuffixArray(const std::string& string,
                                 const std::vector<size_t>& sub_suffix_array,
                                 std::vector<size_t>& new_sub_suffix_array,
                                 size_t k_to_power_of_two) {
    for (size_t i = 0; i < string.size(); ++i) {
      new_sub_suffix_array[i] =
        sub_suffix_array[i] >= k_to_power_of_two ?
        sub_suffix_array[i] - k_to_power_of_two  :
        (string.size() - k_to_power_of_two) + sub_suffix_array[i];
    }
}

std::vector<size_t>
LongestCommonPrefixArray(const std::string& string,
                         const std::vector<size_t>& suffix_array) {
  std::vector<size_t> longest_common_prefix_array(suffix_array.size());


  std::vector<size_t> inverse_suffix_array(suffix_array.size());
  for (size_t i = 0; i < suffix_array.size(); ++i) {
    inverse_suffix_array[suffix_array[i]] = i;
  }

  size_t current_longest_common_prefix = 0;

  for (size_t i = 0; i < suffix_array.size(); ++i) {
    if (current_longest_common_prefix > 0) --current_longest_common_prefix;

    if (inverse_suffix_array[i] == 0) {
      current_longest_common_prefix = 0;
      continue;
    } else {

      int64_t compared = suffix_array[inverse_suffix_array[i] - 1];
      while ((std::max(i + current_longest_common_prefix,
                       compared + current_longest_common_prefix) <
              suffix_array.size()) &&
             (string[i + current_longest_common_prefix] ==
              string[compared + current_longest_common_prefix])) {
        ++current_longest_common_prefix;
      }
      longest_common_prefix_array[inverse_suffix_array[i]] =
        current_longest_common_prefix;
    }
  }

  return longest_common_prefix_array;
}

void
UpdateClasses(const std::string& string,
              const std::vector<size_t>& sub_suffix_array,
              std::vector<size_t>& classes,
              std::vector<size_t>& new_classes,
              size_t& classes_count,
              size_t k_to_power_of_two) {
  new_classes[sub_suffix_array[0]] = 0;
  classes_count = 1;
  for (size_t i = 0; i < string.size() - 1; ++i) {
    if (
        (classes[sub_suffix_array[i]] !=
         classes[sub_suffix_array[i + 1]]) ||
        (classes[(sub_suffix_array[i] + k_to_power_of_two) % string.size()] !=
         classes[(sub_suffix_array[i + 1] + k_to_power_of_two) % string.size()])
       ) {
      ++classes_count;
    }
    new_classes[sub_suffix_array[i + 1]] = classes_count - 1;
  }

  std::swap(classes, new_classes);
}

void SubCountingSortViaClasses(const std::string& string,
                               std::vector<size_t>& sub_suffix_array,
                               const std::vector<size_t>& new_sub_suffix_array,
                               const std::vector<size_t>& classes,
                               std::vector<size_t>& classes_counter,
                               size_t classes_count) {
    std::fill(classes_counter.begin(),
              classes_counter.begin() + classes_count, 0);

    for (size_t i = 0; i < string.size(); ++i) {
      ++classes_counter[classes[new_sub_suffix_array[i]]];
    }

    for (size_t i = 0; i < classes_count - 1; ++i) {
      classes_counter[i + 1] += classes_counter[i];
    }

    for (int64_t i = string.size() - 1; i >= 0; --i) {
      --classes_counter[classes[new_sub_suffix_array[i]]];
      sub_suffix_array[classes_counter[classes[new_sub_suffix_array[i]]]] =
        new_sub_suffix_array[i];
    }
}

size_t
CalculateDifferentSubstringsCount(const std::string& string) {
  auto suffix_array = SuffixArray(string);
  auto longest_common_prefix_array =
    LongestCommonPrefixArray(string, suffix_array);

  size_t different_substrings_count = string.size() - suffix_array[0];

  for (size_t i = 1; i < string.size(); ++i) {
    different_substrings_count +=
      string.size() - suffix_array[i] - longest_common_prefix_array[i];
  }

  return different_substrings_count;
}
