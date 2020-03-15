#pragma once
#include "StringFunctionsConversions.h"

std::vector<size_t> PrefixFunction(const std::string& line) {
  std::vector<size_t> prefix_function(line.size(), 0);

  for (size_t i = 1 ; i < line.size(); ++i) {
    size_t prev_func_value = prefix_function[i - 1];

    while (line[prev_func_value] != line[i] &&
           prev_func_value != 0) {
      prev_func_value = prefix_function[prev_func_value - 1];
    }

    if (prev_func_value == 0) {
      prefix_function[i] = (line[i] == line[0]) ? 1 : 0;
    } else {
      prefix_function[i] = prev_func_value + 1;
    }
  }

  return prefix_function;
}
