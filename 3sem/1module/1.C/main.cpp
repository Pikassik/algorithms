/* C. Шаблон с ?
 * Шаблон поиска задан строкой длины m, в которой кроме
 * обычных символов могут встречаться символы “?”. Найти
 * позиции всех вхождений шаблона в тексте длины n. Каждое
 * вхождение шаблона предполагает, что все обычные символы
 * совпадают с соответствующими из текста, а вместо символа “?”
 * в тексте встречается произвольный символ.
 * Время работы - O(n + m + Z), где Z - общее -число вхождений
 * подстрок шаблона “между вопросиками” в исходном тексте.
 * m ≤ 5000, n ≤ 2000000.
 */

#include <iostream>
#include <vector>
#include <map>
#include <cstdint>
#include <queue>
#include <cmath>

class Trie {
 public:
  Trie() = delete;
  explicit Trie(const std::vector<std::string>& dict);

  size_t Delta(size_t vertex, char letter);
  static inline bool IsAbcent(int64_t link);
  static inline bool IsRoot(size_t vertex);

  struct Node {
   std::map<char, size_t> next;
   std::vector<size_t>    pattern_numbers;
   size_t                 depth                  = 0;
   size_t                 parent                 = root;
   size_t                 suffix_link            = root;
   int64_t                compressed_suffix_link = abcent;
   bool                   is_terminal            = false;
   char                   char_to_parent         = '\0';

   Node(int64_t depth,
        int64_t parent,
        bool is_terminal,
        char char_to_parent);
  };

  static const int root = 0;
  static const int abcent = -1;

  std::vector<Node> buffer_;
 private:
  int64_t Up(size_t vertex);
  void BuildTrie(const std::vector<std::string>& dict);
  void BuildSuffixLinks();
};

struct SubUsages {
  std::vector<uint32_t> sub_usages_counters;
  size_t                dict_size;
};

std::vector<std::string> ParseToPatterns(const std::string& pattern, std::vector<size_t>& string_indexes);

SubUsages FindSubUsages(const std::string& pattern, const std::string& text);

void PrintPatternUsages(SubUsages sub_usages);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::string pattern;
  std::cin >> pattern;

  std::string text;
  std::cin >> text;

  PrintPatternUsages(FindSubUsages(pattern, text));

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

Trie::Trie(const std::vector<std::string>& dict)
    : buffer_(1, Node(0, root, false, '\0')) {
  BuildTrie(dict);
  BuildSuffixLinks();
}

Trie::Node::Node(int64_t depth,
                 int64_t parent,
                 bool is_terminal,
                 char char_to_parent)
    : depth          (depth)
    , parent         (parent)
    , is_terminal    (is_terminal)
    , char_to_parent (char_to_parent) {}

bool Trie::IsAbcent(int64_t link) {
  return link == abcent;
}

bool Trie::IsRoot(size_t vertex) {
  return vertex == root;
}

size_t Trie::Delta(size_t vertex, char letter) {
  if (buffer_[vertex].next.find(letter) != buffer_[vertex].next.end()) {
    return buffer_[vertex].next[letter];
  }

  if (IsRoot(vertex) &&
      buffer_[vertex].next.find(letter) == buffer_[vertex].next.end()) {
    return root;
  }

  return Delta(buffer_[vertex].suffix_link, letter);
}

int64_t Trie::Up(size_t vertex) {
  if (buffer_[buffer_[vertex].suffix_link].is_terminal) {
    return buffer_[vertex].suffix_link;
  }

  if (IsRoot(buffer_[vertex].suffix_link)) {
    return -1;
  }

  return Up(buffer_[vertex].suffix_link);
}

void Trie::BuildTrie(const std::vector<std::string>& dict) {
  for (size_t i = 0; i < dict.size(); ++i) {

    size_t current = root;
    for (auto letter: dict[i]) {

      if (buffer_[current].next.find(letter) == buffer_[current].next.end()) {

        buffer_.emplace_back(buffer_[current].depth + 1,
                             current,
                             false,
                             letter);
        buffer_[current].next[letter] = buffer_.size() - 1;

        current = buffer_.size() - 1;

      } else {
        current = buffer_[current].next[letter];
      }
    }

    buffer_[current].is_terminal = true;
    buffer_[current].pattern_numbers.push_back(i);
  }
}

void Trie::BuildSuffixLinks() {

  std::queue<size_t> queue;
  for (const auto& vertex_from_root: buffer_[root].next) {

    for (const auto& next_vertex: buffer_[vertex_from_root.second].next) {
      queue.push(next_vertex.second);
    }

  }

  while (!queue.empty()) {

    size_t current = queue.front();
    queue.pop();

    buffer_[current].suffix_link =
        Delta(buffer_[buffer_[current].parent].suffix_link,
              buffer_[current].char_to_parent);

    buffer_[current].compressed_suffix_link = Up(current);

    for (const auto& next_vertex: buffer_[current].next) {
      queue.push(next_vertex.second);
    }

  }
}

std::vector<std::string> ParseToPatterns(const std::string& pattern, std::vector<size_t>& string_indexes) {
  std::vector<std::string> patterns;

  for (size_t i = 0; i < pattern.size(); ++i) {

    while (i < pattern.size() && pattern[i] == '?') ++i;

    int string_index = i;
    string_indexes.push_back(string_index);

    while (i < pattern.size() && pattern[i] != '?') ++i;

    std::string new_string(pattern.c_str() + string_index, i - string_index);
    if (!new_string.empty()) {
      patterns.push_back(std::move(new_string));
    }
  }

  return patterns;
}

SubUsages FindSubUsages(const std::string& pattern, const std::string& text) {
  if (pattern.empty() || pattern.size() > text.size()) {
    return {{}, 0};
  }

  std::vector<size_t> string_indexes;
  std::vector<std::string> dict = ParseToPatterns(pattern, string_indexes);
  Trie trie(dict);
  size_t dict_size = dict.size();
  dict.clear();
  std::vector<uint32_t> sub_usages(text.size() - pattern.size() + 1);

  size_t current_vertex = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    current_vertex = trie.Delta(current_vertex, text[i]);
    if (trie.buffer_[current_vertex].is_terminal) {
      for (auto pattern_index: trie.buffer_[current_vertex].pattern_numbers) {
        size_t usage = (i + 1) - trie.buffer_[current_vertex].depth;
        if (usage >= string_indexes[pattern_index] &&
            usage -  string_indexes[pattern_index] < sub_usages.size()) {
          ++sub_usages[usage - string_indexes[pattern_index]];
        }
      }
    }

    int64_t current_compressed = trie.buffer_[current_vertex].compressed_suffix_link;
    while (!Trie::IsAbcent(current_compressed)) {

      for (auto pattern_index: trie.buffer_[current_compressed].pattern_numbers) {
        size_t usage = (i + 1) - trie.buffer_[current_compressed].depth;
        if (usage >= string_indexes[pattern_index] &&
            usage -  string_indexes[pattern_index] < sub_usages.size()) {
          ++sub_usages[usage - string_indexes[pattern_index]];
        }
      }

      current_compressed = trie.buffer_[current_compressed].compressed_suffix_link;
    }
  }

  return {std::move(sub_usages), dict_size};
}

void PrintPatternUsages(SubUsages sub_usages) {
  for (size_t i = 0; i < sub_usages.sub_usages_counters.size(); ++i) {
    if (sub_usages.sub_usages_counters[i] == sub_usages.dict_size) {
      std::cout << i << ' ';
    }
  }
}
