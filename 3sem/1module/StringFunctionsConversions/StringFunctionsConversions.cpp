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

std::vector<size_t> ZFunction(const std::string& line) {
  std::vector<size_t> z_function(line.size());
  size_t left = 0;
  size_t right = 0;
  z_function[0] = line.size();

  for (size_t i = 1; i < line.size(); ++i) {
    if (i > right) {
      size_t j = 0;
      for ( ; line[i + j] == line[j]; ++j);
      z_function[i] = j;
      left = i;
      right = i + j - 1;
    } else if (right < z_function[i - left] + i) {
      for ( ; line[right] == line[right - i]; ++right);
      z_function[i] = right - i;
    } else {
      z_function[i] = z_function[i - left];
    }
  }

  return z_function;
}

std::vector<size_t> PrefixFunctionFromZFunction(const std::vector<size_t>&
                                                z_function) {
  std::vector<size_t> prefix_function(z_function.size(), 0);

  for (size_t i = 1; i < z_function.size(); ++i) {
    for (int64_t j = z_function[i] - 1; j >= 0; --j) {
      if (prefix_function[i + j] > 0) {
        break;
      } else {
        prefix_function[i + j] = j + 1;
      }
    }
  }

  return prefix_function;
}

std::vector<size_t> ZFunctionFromPrefixFunction(const std::vector<size_t>&
                                                prefix_function) {
  return ZFunction(MinStringFromPrefixFunction(prefix_function));
}

std::string MinStringFromPrefixFunction(const std::vector<size_t>&
                                        prefix_function) {
  std::string result_string(prefix_function.size(), 'a');

  std::unordered_set<char> char_set('z' - 'a' + 1);
  for (size_t i = 1; i < prefix_function.size(); ++i) {
    if (prefix_function[i] == 0) {
      for (size_t j = prefix_function[i - 1];
           j != 0;
           j = prefix_function[j - 1]) {
        char_set.insert(result_string[j]);
      }

      char last_char = 'b';
      for ( ;
          last_char != 'z' &&
          char_set.find(last_char) != char_set.end();
          ++last_char);

      result_string[i] = last_char;
      char_set.clear();
    } else {
      result_string[i] = result_string[prefix_function[i] - 1];
    }
  }

  return result_string;
}

std::string MinStringFromZFunction(const std::vector<size_t>& z_function) {
  return MinStringFromPrefixFunction(PrefixFunctionFromZFunction(z_function));
}
