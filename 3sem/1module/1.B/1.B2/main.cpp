/* B2. Строка по Z-функции
 * Найти лексикографически-минимальную строку,
 * построенную по z-функции, в алфавите a-z.
 */

#include "../../StringFunctionsConversions/StringFunctionsConversions.h"
#include <iostream>

int main() {
  std::vector<size_t> prefix_functions;
  size_t value;
  
  while (std::cin >> value) {
    prefix_functions.push_back(value);
  }

  std::cout << MinStringFromZFunction(prefix_functions);
  return 0;
}