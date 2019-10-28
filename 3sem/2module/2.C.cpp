#include <iostream>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string_view>

class SuffixTree {
 public:
  SuffixTree(const std::string& string);
  SuffixTree(std::string&& string);
  const std::map<char, size_t>& NextVertices(size_t vertex) const;
  inline size_t GetLeftBound(size_t vertex) const;
  inline size_t GetRightBound(size_t vertex) const;
  const std::string& GetString() const;
  size_t Size() const;

 private:
  struct Node {
    Node() = delete;
    Node(size_t left_bound,
         size_t right_bound,
         bool is_leaf);

    // [left_bound, right_bound]
    size_t left_bound;
    size_t right_bound;
    bool   is_leaf;
    size_t suffix_link = 0;
    std::map<char, size_t>  next;
  };


  inline bool IsPath(size_t vertex, size_t string_index);
  inline size_t& Next(size_t vertex, size_t string_index);
  void InsertLeaf(size_t parent);
  void CutEdge(size_t current_vertex, size_t current_index);
  void BuildTree();
  size_t leaf_right_bound = 0;
  std::string string_;
  std::vector<Node> buffer_;
};


void GetCommonSubstrings(size_t index,
                         size_t first_string_size,
                         size_t concatenated_strings_size,
                         const SuffixTree& tree,
                         std::vector<std::pair<bool, bool>>& strings_identity);


bool KthLexicographicallyCommonSubstring(size_t index,
                                         size_t statictics_index,
                                         size_t& counter,
                                         size_t& remainder,
                                         std::vector<size_t>& vertices,
                                         const SuffixTree& tree,
                                         const std::vector<std::pair<bool,
                                             bool>>& strings_identity);

std::string RestoreStringFromSequence(const std::vector<size_t>& sequence,
                                      const SuffixTree& tree,
                                      size_t remainder);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::string first_string, second_string;
  size_t statistics_index;
  std::cin >> first_string >> second_string >> statistics_index;
  first_string.push_back('#');
  second_string.push_back('$');

  auto suffix_tree = SuffixTree(std::move(first_string + second_string));

  std::vector<std::pair<bool, bool>> strings_identity =
      std::vector<std::pair<bool, bool>>(suffix_tree.Size(),
                                         std::make_pair(false, false));

  GetCommonSubstrings(0,
                      first_string.size(),
                      suffix_tree.GetString().size(),
                      suffix_tree,
                      strings_identity);

  size_t counter = 0;
  size_t remainder = 0;
  std::vector<size_t> vertices;
  KthLexicographicallyCommonSubstring(0,
                                      statistics_index,
                                      counter,
                                      remainder,
                                      vertices,
                                      suffix_tree,
                                      strings_identity);

  if (vertices.empty()) {
    std::cout << -1;
  } else {
    std::cout << RestoreStringFromSequence(vertices, suffix_tree, remainder);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

SuffixTree::SuffixTree(const std::string& string)
: string_(string)
, buffer_(std::vector<Node>(1, Node(0, 0, false))) {
  BuildTree();
}


SuffixTree::SuffixTree(std::string&& string)
: string_(std::move(string))
, buffer_(std::vector<Node>(1, Node(0, 0, false))) {
  BuildTree();
}

const std::map<char, size_t>& SuffixTree::NextVertices(size_t vertex) const {
  return buffer_[vertex].next;
}

size_t SuffixTree::GetLeftBound(size_t vertex) const {
  return buffer_[vertex].left_bound;
}

size_t SuffixTree::GetRightBound(size_t vertex) const {
  return buffer_[vertex].is_leaf ?
         leaf_right_bound :
         buffer_[vertex].right_bound;
}

const std::string& SuffixTree::GetString() const {
  return string_;
}

size_t SuffixTree::Size() const {
  return buffer_.size();
}

SuffixTree::Node::Node(size_t left_bound,
                       size_t right_bound,
                       bool is_leaf)
: left_bound(left_bound)
, right_bound(right_bound)
, is_leaf(is_leaf) {}

bool SuffixTree::IsPath(size_t vertex, size_t string_index) {
  return buffer_[vertex].next.find(string_[string_index]) !=
    buffer_[vertex].next.end();
}

size_t& SuffixTree::Next(size_t vertex, size_t string_index) {
  return buffer_[vertex].next[string_[string_index]];
}

void SuffixTree::InsertLeaf(size_t parent) {
  buffer_[parent].next[string_[leaf_right_bound]] = buffer_.size();
  buffer_.emplace_back(leaf_right_bound, leaf_right_bound, true);
}

void SuffixTree::CutEdge(size_t current_vertex, size_t current_index) {
  size_t cut_edge_left_bound =
    buffer_[Next(current_vertex, current_index)].left_bound;
  size_t old_child = Next(current_vertex, current_index);

  buffer_[Next(current_vertex, current_index)].left_bound =
    buffer_[Next(current_vertex, current_index)].left_bound +
    leaf_right_bound - current_index;

  buffer_.emplace_back(cut_edge_left_bound,
                       cut_edge_left_bound + leaf_right_bound -
                       current_index - 1,
                       false
                      );
  buffer_.emplace_back(leaf_right_bound, leaf_right_bound, true);

  Next(buffer_.size() - 2, leaf_right_bound) = buffer_.size() - 1;
  Next(buffer_.size() - 2,
       cut_edge_left_bound + leaf_right_bound - current_index) =
    old_child;
  Next(current_vertex, current_index) = buffer_.size() - 2;
}

void SuffixTree::BuildTree() {

  size_t current_index = 0;
  int64_t previous_vertex = -1;
  size_t current_vertex = 0;
  size_t j = 0;
  for (size_t i = 0; i < string_.size(); ++i) {
    // while building always equals to i
    leaf_right_bound = i;

    for ( ; j <= i; ++j) {

      while (IsPath(current_vertex, current_index) &&
             GetRightBound(Next(current_vertex, current_index)) -
             GetLeftBound(Next(current_vertex, current_index)) <
             i - current_index) {
        size_t old_index = current_index;
        current_index +=
          GetRightBound(Next(current_vertex, current_index)) -
          GetLeftBound(Next(current_vertex, current_index)) + 1;
        current_vertex = Next(current_vertex, old_index);
      }

      if (current_index == i &&
          IsPath(current_vertex, i)) {
        if (previous_vertex != -1) {
          buffer_[previous_vertex].suffix_link = current_vertex;
          previous_vertex = -1;
        }
      }

      if (current_index == i &&
          !IsPath(current_vertex, i)) {
        if (previous_vertex != -1) {
          buffer_[previous_vertex].suffix_link = current_vertex;
          previous_vertex = -1;
        }

        if (current_vertex == 0) ++current_index;
        InsertLeaf(current_vertex);
        current_vertex = buffer_[current_vertex].suffix_link;
        continue;
      }


      if (string_[GetLeftBound(Next(current_vertex, current_index)) +
                  i - current_index] ==
          string_[i]) {
        break;
      }

      if (string_[GetLeftBound(Next(current_vertex, current_index)) +
                  i - current_index] !=
          string_[i]) {

        if (previous_vertex != -1) {
          buffer_.at(previous_vertex).suffix_link = buffer_.size();
          previous_vertex = -1;
        }
        previous_vertex = buffer_.size();

        CutEdge(current_vertex, current_index);

        if (current_vertex == 0) ++current_index;
        current_vertex = buffer_[current_vertex].suffix_link;
      }
    }

  }
}


bool KthLexicographicallyCommonSubstring(size_t index,
                                         size_t statictics_index,
                                         size_t& counter,
                                         size_t& remainder,
                                         std::vector<size_t>& vertices,
                                         const SuffixTree& tree,
                                         const std::vector<std::pair<bool,
                                             bool>>& strings_identity) {

    for (auto x: tree.NextVertices(index)) {
      if (strings_identity[x.second].first &&
          strings_identity[x.second].second) {
        vertices.push_back(x.second);
        counter += tree.GetRightBound(x.second) -
          tree.GetLeftBound(x.second) + 1;

        if (counter >= statictics_index) {
          remainder = tree.GetRightBound(x.second) -
          tree.GetLeftBound(x.second) + 1 - (counter - statictics_index);
          return true;
        }

        if (KthLexicographicallyCommonSubstring(x.second,
                                                statictics_index,
                                                counter,
                                                remainder,
                                                vertices,
                                                tree,
                                                strings_identity)) {
          return true;
        }

        vertices.pop_back();
      }
    }

  return false;
}


void GetCommonSubstrings(size_t index,
                         size_t first_string_size,
                         size_t concatenated_strings_size,
                         const SuffixTree& tree,
                         std::vector<std::pair<bool, bool>>& strings_identity) {

  for (auto x: tree.NextVertices(index)) {

    if (tree.GetLeftBound(x.second) <= first_string_size - 1 &&
        first_string_size - 1 <= tree.GetRightBound(x.second)) {
      strings_identity[index].first = true;
      continue;
    }

    if (tree.GetRightBound(x.second) == concatenated_strings_size - 1) {
      strings_identity[index].second = true;
    }

    GetCommonSubstrings(x.second,
                        first_string_size,
                        concatenated_strings_size,
                        tree,
                        strings_identity);

    strings_identity[index].first |= strings_identity[x.second].first;
    strings_identity[index].second |= strings_identity[x.second].second;
  }
}

std::string RestoreStringFromSequence(const std::vector<size_t>& sequence,
                                      const SuffixTree& tree,
                                      size_t remainder) {
  std::string restored_string;
  for (size_t i = 0; i < sequence.size() - 1; ++i) {
    restored_string += static_cast<std::string_view>(tree.GetString()).
      substr(tree.GetLeftBound(sequence[i]),
             tree.GetRightBound(sequence[i]) + 1 -
             tree.GetLeftBound(sequence[i]));
  }

  restored_string += static_cast<std::string_view>(tree.GetString()).
    substr(tree.GetLeftBound(sequence.back()), remainder);

  return restored_string;
}
