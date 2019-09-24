/* B2. Строка по Z-функции
 * Найти лексикографически-минимальную строку,
 * построенную по z-функции, в алфавите a-z.
 */

#include "../../StringFunctionsConversions/StringFunctionsConversions.h"
#include <iostream>

int main() {
  std::vector<size_t> z_function;
  size_t value;
  
  while (std::cin >> value) {
    z_function.push_back(value);
  }

  std::cout << MinStringFromZFunction(z_function);
  return 0;
}