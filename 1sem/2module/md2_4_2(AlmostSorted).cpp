// Задание 4.2 Сортировка почти упорядоченной последовательности
// Дана последовательность целых чисел a1...an и натуральное число k, такое что для любых i, j:
// если j >= i + k, то a[i] <= a[j]. Требуется отсортировать последовательность.
// Последовательность может быть очень длинной. Время работы O(n * log(k)).
// Доп. память O(k). Использовать слияние.
#include <iostream>
#include <vector>

using std::cout;
using std::cin;
using std::vector;

void MergeSort(vector<int>&, int, int);

void Merge(const vector<int>&, vector<int>&, int, int, int);

///////////////////////////////////////////////////////////////////////////////

int main() {
    vector<int> inputarray;
    int n, k;
    cin >> n >> k;
    inputarray.reserve(2 * k);
    for (int i = 0; (i < 2 * k) && (i < n); ++i) {                 // ввод первых 2k элементов
        int input;
        cin >> input;
        inputarray.push_back(input);
    }
    MergeSort(inputarray, 0, static_cast<int>(inputarray.size())); // сортировка первых 2k символов
    for (int i = 0; i < k; ++i) {                                  // вывод первых k символов
        cout << inputarray[i] << ' ';
    }
    for (int i = 0; i < n / k - 2; ++i) {                          // ввод, сортировка и вывод основной части массива
        for (int i = 0; i < k; ++i) {                              // ввод следующих 2k элементов вместо выведенных
            int input;
            cin >> input;
            inputarray[i] = input;
        }
        MergeSort(inputarray, 0, static_cast<int>(inputarray.size())); // сортировка старых k и новых k элементов
        for (int i = 0; i < k; ++i) {                                  // вывод следующих k элементов
            cout << inputarray[i] << ' ';
        }
    }
    vector<int> outputarray;                                       // ввод/вывод/сортировка "хвоста"
    outputarray.reserve(n % k + k);
    for (int i = k; i < static_cast<int>(inputarray.size()); ++i) {
        outputarray.push_back(inputarray[i]);
    }
    inputarray.clear();
    for (int i = 0; i < n % k; ++i) {
        int input;
        cin >> input;
        outputarray.push_back(input);
    }
    MergeSort(outputarray, 0, static_cast<int>(outputarray.size()));
    for (int i = 0; i < n % k + k; ++i) {
        cout << outputarray[i] << ' ';
    }
}

///////////////////////////////////////////////////////////////////////////////

void MergeSort(vector<int> &array, int begin, int end) {
    if (end - begin <= 1) {
        return;
    }
    int border = (end - begin) / 2 + begin;
    MergeSort(array, begin, border);
    MergeSort(array, border, end);
    vector<int> merged(end - begin);
    Merge(array, merged, border, begin, end);
    for (int i = begin; i < end; ++i) {
        array[i] = merged[i - begin];
    }
}

void Merge(const vector<int> &array, vector<int> &merged, int border, int begin, int end) {
    int i = 0, j = 0;
    while (i < border - begin && j < end - border) {
        if (array[begin + i] <= array[border + j]) {
            merged[i + j] = array[begin + i];
            ++i;
        } else {
            merged[i + j] = array[border + j];
            ++j;
        }
    }
    if (begin + i == border) {
        for ( ; border + j < end; ++j) {
            merged[i + j] = array[border + j];
        }
    } else {
        for ( ; begin + i < border; ++i) {
            merged[i + j] = array[begin + i];
        }
    }
}