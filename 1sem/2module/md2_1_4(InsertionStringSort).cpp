//1.4 Строки.
//Напишите программу, печатающую набор строк в лексикографическом порядке.
//Строки разделяются символом перевода строки ’∖n’. Если последний символ в
// потоке ввода ’∖n’, считать, что после него нет пустой строки. Известно,
// что либо n<100, либо n≤1000, но при этом число инверсий в данном наборе
// строк меньше 100. Максимальная длина строки 255 символов. Написать свою
// функцию сравнения строк. Время работы O(n^2), доп. память O(1).
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using std::swap;
using std::vector;
using std::string;
using std::endl;
using std::cin;
using std::cout;

class Compare {                     //функтор, сравнивающий свою строку с заданной
public:
    bool operator()(string &left, string &right);     //оператор имеет то же значение, что и value_ < current
};

void StringSort(vector<string> &array);

///////////////////////////////////////////////////////////////////////////////

int main() {
    vector<string> inputarray;            //ввод/вывод данных + вызов функции сортировки
    int amount;
    cin >> amount;
    inputarray.reserve(amount);
    for (int i = 0; i < amount; ++i) {
        string input;
        cin >> input;
        inputarray.push_back(input);
    }
    StringSort(inputarray);
    for (int i = 0; i < amount; ++i) {
        cout << inputarray[i] << endl;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Compare::operator()(string &left, string &right) {
    for(int i = 0; right[i] != 0; ++i) {
        if (left[i] == right[i]) {
            continue;
        }  else if (left[i] < right[i]) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void StringSort(vector<string> &array) {                               //реализация через сортировку вставками
    Compare isless;
    for (int i = 1; i < static_cast<int>(array.size()); ++i) {
        string copy = array[i];
        for(int j = i - 1; (j >= 0) && isless(copy, array[j]); --j) {  // < заменено на функтор isless
            swap(array[j], array[j + 1]);
        }
    }
}


