#pragma once
#include <vector>

class DSU {
 public:
  DSU() = delete;
  DSU(size_t size);
  size_t Get(size_t element);
  void Union(size_t first, size_t second);
 protected:
  std::vector<size_t> buffer_;
  std::vector<int> ranks_;
};