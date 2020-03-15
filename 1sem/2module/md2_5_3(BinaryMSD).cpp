//Задача 5.3 Binary MSD для long long.
//Дан массив неотрицательных целых 64-разрядных чисел. Количество чисел не больше 1000000.
//Отсортировать массив методом MSD по битам (бинарный QuickSort).
#include <iostream>
#include <vector>

using ull = unsigned long long;
using std::swap;
using std::cin;
using std::cout;
using std::vector;
using std::endl;

void Sort(vector<ull>&, int, int, int);

int Partition(vector<ull>&, int, int, int);


///////////////////////////////////////////////////////////////////////////////

int main() {
    vector<ull> inputarray;
    int n;
    cin >> n;
    inputarray.reserve(n);
    for (int i = 0; i < n; ++i) {
        ull input;
        cin >> input;
        inputarray.push_back(input);
    }
    Sort(inputarray, 0, n, sizeof(ull) * 8);
    for (int i = 0; i < n; ++i) {
        cout << inputarray[i] << ' ';
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void Sort(vector<ull> &array, int begin, int end, int bit) { //Реализация обычного QuickSort, каждый новый вызов
    int pivot_position = Partition(array, begin, end, bit);  //QuickSort понижает позицию разряда на единицу
    if ((pivot_position > begin) && (bit > 1)) {
        Sort(array, begin, pivot_position, bit - 1);
    }
    if ((pivot_position < end - 1) && (bit > 1)) {
        Sort(array, pivot_position, end, bit - 1);
    }
}

int Partition(vector<ull> &array , int begin, int end, int bit) {
    ull pivot = 1LL << static_cast<ull>((bit - 1));               //За опорный элемент берется 2^(разряд)
    if (end - begin <= 1) {                                       //Выполняется стандартный Partition
        return begin;
    }
    int i = begin;
    int j = end - 1;
    while (i <= j) {
        while ((i < end) && ((array[i] & pivot) == 0)) {          //Сравниваются не сами элементы, а только
            ++i;                                                  //соответствующие разряды
        }
        while ((j >= begin) && ((array[j] & pivot) == pivot)) {
            --j;
        }
        if (i < j) {
            swap(array[i], array[j]);
        }
    }
    return i;
}