/* A. Поиск подстроки
 * Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n.
 * Время O(n + p), доп. память – O(p).
 * p <= 30000, n <= 300000.
 *
 * Использовать один из методов:
 * - С помощью префикс-функции;
 * - С помощью z-функции.
 */

#include <iostream>
#include <string>
#include <vector>
#include <../StringFunctionsConversions/StringFunctionsConversions.h>

std::vector<size_t> UsagesIndexes(const std::string& pattern,
                                  const std::string& text);

///////////////////////////////////////////////////////////////////////////////

int main() {
  std::string pattern;
  std::cin >> pattern;

  std::string text;
  std::cin >> text;

  for (auto x: UsagesIndexes(pattern, text)) {
    std::cout << x << " ";
  }
}

///////////////////////////////////////////////////////////////////////////////

std::vector<size_t> UsagesIndexes(const std::string& pattern,
                                  const std::string& text) {
  std::vector<size_t> usages_indexes;
  std::vector<size_t> pattern_prefix_function = PrefixFunction(pattern);

  size_t prev_func_value = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    while (pattern[prev_func_value] != text[i] &&
           prev_func_value != 0) {
      prev_func_value = pattern_prefix_function[prev_func_value - 1];
    }

    if (prev_func_value == 0) {
      prev_func_value = (text[i] == pattern[0]) ? 1 : 0;
    } else {
      prev_func_value = prev_func_value + 1;
    }

    if (prev_func_value == pattern.size()) {
      usages_indexes.push_back(i - pattern.size() + 1);
    }
  }

  return usages_indexes;
}
