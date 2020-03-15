#include <headers/DSU.h>

DSU::DSU(size_t size)
    : buffer_(size)
      , ranks_(size, 0) {
  for (size_t i = 0; i < size; ++i) {
    buffer_[i] = i;
  }
}

size_t DSU::Get(size_t element) {
  if (buffer_[element] != element) {
    buffer_[element] = Get(buffer_[element]);
  }

  return buffer_[element];
}

void DSU::Union(size_t first, size_t second) {
  size_t get_first = Get(first);
  size_t get_second = Get(second);
  if (ranks_[get_first] == ranks_[get_second]) {
    ++ranks_[get_first];
  }

  if (ranks_[get_first] < ranks_[get_second]) {
    buffer_[get_first] = buffer_[get_second];
  } else {
    buffer_[get_second] = buffer_[get_first];
  }
}