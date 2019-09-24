/* B1. Строка по префикс-функции
 * Найти лексикографически-минимальную строку,
 * построенную по префикс-функции, в алфавите a-z.
 */

#include "../../StringFunctionsConversions/StringFunctionsConversions.h"
#include <iostream>

int main() {
  std::vector<size_t> prefix_function;
  size_t value;
  
  while (std::cin >> value) {
    prefix_function.push_back(value);
  }

  std::cout << MinStringFromPrefixFunction(prefix_function);
  return 0;
}