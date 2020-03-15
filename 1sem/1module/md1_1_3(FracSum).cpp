//Сумма дробей
//Даны четыре неотрицательных числа a, b, c и d. Сложить две рациональные дроби a/b и c/d, а их результат представить
// в виде несократимой дроби m/n. Вывести числа m и n. a, b, c, d ≤ 1000.
// Время работы O(log(b*d))
#include <iostream>

void SumOfFractionalNumbers(int a, int b, int c, int d, int &numerator, int &denominator) {
    numerator = a * d + b * c; //приведение дроби к общему знаменателю
    denominator = b * d;
    for(int i = 2; i <= std::min(numerator, denominator); ++i){ //проверка сократимости дроби и сокращение ее
        if ((numerator % i == 0) && (denominator % i == 0)) {
            numerator /= i;
            denominator /= i;
            --i;
        }
    }
}

int main() {
    int a, b, c, d;
    std::cin >> a >> b >> c >> d;
    int numerator = 0;
    int denominator = 0;
    SumOfFractionalNumbers(a, b, c, d, numerator, denominator);
    std::cout << numerator << ' ' << denominator;
    return 0;
}