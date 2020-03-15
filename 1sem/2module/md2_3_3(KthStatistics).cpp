//Задача 3.3 Порядковая статистика.
//Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..109] размера n.
//Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции
//с индексом k ∈[0..n-1] в отсортированном массиве.
//Напишите нерекурсивный алгоритм.
//Требования к дополнительной памяти: O(n).
//Требуемое среднее время работы: O(n).
//Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.
//Описание для случая прохода от начала массива к концу:
//Выбирается опорный элемент.
//Опорный элемент меняется с последним элементом массива.
//Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. Затем располагаются элементы,
//строго бОльшие опорного. В конце массива лежат нерассмотренные элементы. Последним элементом лежит опорный.
//Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
//Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
//Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j.
//Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
//В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.
//Реализуйте стратегию выбора опорного элемента “случайный элемент”.
//Функцию Partition реализуйте методом прохода двумя итераторами от начала массива к концу.

#include <iostream>
#include <vector>
#include <cstdlib>
#include <random>

using std::cin;
using std::cout;
using std::swap;
using std::endl;
using std::vector;

int KStatistics(int, vector<int> &array);

int Partition(int , int, vector<int> &array);

int rnd(int, int);

///////////////////////////////////////////////////////////////////////////////

int main() {
    vector<int> inputarray;
    int n, k;
    cin >> n >> k;
    inputarray.reserve(n);
    for (int i = 0; i < n; ++i) {
        int input;
        cin >> input;
        inputarray.push_back(input);
    }
    cout << KStatistics(k, inputarray);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

int KStatistics(int k, vector<int> &array) {
    int begin = 0;
    int end = static_cast<int>(array.size());
    int pivot_position = Partition(begin, end, array);
    while (pivot_position != k) {                          //если k меньше опорного, то вызываем Partition слева,
        if (pivot_position > k) {                          //иначе справа.
            end = pivot_position;
            pivot_position = Partition(begin, end, array);
        } else {
            begin = pivot_position + 1;
            pivot_position = Partition(begin, end, array);
        }
    }
    return array[k];
}

int Partition(int begin, int end, vector<int> &array) {
    if (end - begin <= 1) {
        return begin;
    }
    swap(array[rnd(begin, end)], array[end - 1]);
    const int pivot = array[end - 1];
    int i = begin;
    int j = begin;
    while (j < end - 1) {
        if (array[j] > pivot) {
            ++j;
        } else {
            swap(array[j], array[i]);
            ++i;
            ++j;
        }
    }
    swap(array[i], array[end - 1]);
    return i;
}

int rnd(int begin, int end) {
    std::random_device rd;
    std::mt19937 mersenne(rd());
    return (mersenne() % (end - begin)) + begin;
}