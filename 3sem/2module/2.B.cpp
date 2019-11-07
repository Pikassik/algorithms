#include <iostream>
#include <map>
#include <vector>
#include <iterator>

class SuffixTree {
 public:
  SuffixTree(const std::string& string);
  SuffixTree(std::string&& string);
  inline const std::map<char, size_t>& NextVertices(size_t vertex) const;
  inline size_t GetLeftBound(size_t vertex) const;
  inline size_t GetRightBound(size_t vertex) const;
  inline const std::string& GetString() const;
  inline size_t Size() const;

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
  void UpdateSuffixLink(int64_t& previous_vertex, size_t current_vertex);
  void GoDown(size_t& current_index, size_t& current_vertex);
  void InsertLeaf(size_t parent);
  void CutEdge(size_t current_vertex, size_t current_index);
  void BuildTree();
  size_t leaf_right_bound = 0;
  std::string string_;
  std::vector<Node> buffer_;
};


struct VertexInfo {
  size_t parent;
  size_t string_number;

  // [left_bound, right_bound]
  size_t left_bound;
  size_t right_bound;
  VertexInfo(size_t parent,
             size_t string_number,
             size_t left_bound,
             size_t right_bound);
};

void SuffixTreeDfs(size_t index,
                   size_t parent,
                   size_t first_string_size,
                   const SuffixTree& tree,
                   std::vector<VertexInfo>& res);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::string string1, string2;
  std::cin >> string1 >> string2;
  size_t first_string_size = string1.size();
  auto suffix_tree = SuffixTree(string1 + string2);

  std::vector<VertexInfo> vertices_info;
  SuffixTreeDfs(0, 0, first_string_size, suffix_tree, vertices_info);

  std::cout << vertices_info.size() << std::endl;
  for (size_t i = 1; i < vertices_info.size(); ++i) {
    std::printf("%zu %zu %zu %zu\n",
                vertices_info[i].parent,
                vertices_info[i].string_number,
                (vertices_info[i].string_number == 1 ?
                (vertices_info[i].left_bound - first_string_size) :
                 vertices_info[i].left_bound),
                (vertices_info[i].string_number == 1 ?
                (vertices_info[i].right_bound - first_string_size) :
                 vertices_info[i].right_bound) + 1);
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

void SuffixTree::UpdateSuffixLink(int64_t& previous_vertex,
                                  size_t current_vertex) {
  if (previous_vertex != -1) {
    buffer_[previous_vertex].suffix_link = current_vertex;
    previous_vertex = -1;
  }
}

void SuffixTree::GoDown(size_t& current_index, size_t& current_vertex) {
  size_t old_index = current_index;
    current_index +=
      GetRightBound(Next(current_vertex, current_index)) -
      GetLeftBound(Next(current_vertex, current_index)) + 1;
    current_vertex = Next(current_vertex, old_index);
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
        GoDown(current_index, current_vertex);
      }

      if (current_index == i &&
          IsPath(current_vertex, i)) {
        UpdateSuffixLink(previous_vertex, current_vertex);
      }

      if (current_index == i &&
          !IsPath(current_vertex, i)) {
        UpdateSuffixLink(previous_vertex, current_vertex);
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
        UpdateSuffixLink(previous_vertex, current_vertex);
        previous_vertex = buffer_.size();
        CutEdge(current_vertex, current_index);
        if (current_vertex == 0) ++current_index;
        current_vertex = buffer_[current_vertex].suffix_link;
      }
    }
  }
}

VertexInfo::VertexInfo(size_t parent,
                       size_t string_number,
                       size_t left_bound,
                       size_t right_bound)
:parent(parent)
,string_number(string_number)
,left_bound(left_bound)
,right_bound(right_bound) {}

void SuffixTreeDfs(size_t index,
                   size_t parent,
                   size_t first_string_size,
                   const SuffixTree& tree,
                   std::vector<VertexInfo>& res) {

  res.emplace_back(parent,
                   tree.GetLeftBound(index) >= first_string_size ? 1 : 0,
                   tree.GetLeftBound(index),
                   tree.GetRightBound(index));

  if (index != 0 &&
      tree.GetLeftBound(index) <= first_string_size - 1 &&
      first_string_size - 1 <= tree.GetRightBound(index)) {
    res.back().right_bound = first_string_size - 1;
    return;
  }

  size_t current_vertex = res.size() - 1;

  for (auto next_vertex: tree.NextVertices(index)) {
    SuffixTreeDfs(next_vertex.second,
                  current_vertex,
                  first_string_size,
                  tree,
                  res);
  }
}
