//Пересечение множеств
//Даны два массива неповторяющихся целых чисел, упорядоченные по возрастанию. A[0..n-1] и B[0..m-1]. n » m.
// Найдите их пересечение. Требуемое время работы: O(m * log k), где k - позиция элементта B[m-1] в массиве A..
// В процессе поиска очередного элемента B[i] в массиве A пользуйтесь результатом поиска элемента B[i-1]. n, k ≤ 10000.
//Время работы соответствует требованиям, Затрачиваемая память M(n+m)
#include <iostream>

int BinarySearch(const int *array, int flexposition, int count, int element) { //Двоичный поиск, ищущий элемент в массиве
    int first = flexposition, last = count;                                    //и возвращающий либо его вхождение, либо -1,
    while (first < last) {                                                     //если не находит его
        int mid = (first + last) / 2;
        if (element <= array[mid]) {
            last = mid;
        } else {
            first = mid + 1;
        }
    }
    return (first == count || array[first] != element) ? -1 : first;
}

int BinarySearchX(const int *array, int flexposition, int count, int element) {//Двоичный поиск, ищущий элемент в
    int first = flexposition, last = count;                                    //и возвращающий индекс минимального
    while(first < last){                                                       //элемента, не меньшего данного
        int mid = (first + last) / 2;
        if (element > array[mid]){
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    if (first == count){
        return count - 1;
    } else {
        return first;
    }
}

void OverlapOfArrays(int n, int m, const int * A, const int * B) {
    int flexposition = 0;                   //позиция, начиная с которой каждый раз будет производиться поиск переменной
    int k = BinarySearchX(A, flexposition, n, B[m-1]);     //поиск k за O(log(n))
    if (k == -1)                                           // если B[m-1] не найдено, то ищем переменные во всем массиве
        k = n - 1;
    int first = 0;
    for(int i = 0; i < m; ++i){
        if ((first = BinarySearch(A, flexposition, k+1, B[i])) != -1) {
            std::cout << B[i] << ' ';
            flexposition = first;                                             //поиск элементов и запоминание их позиции
        }
    }
}

int main() {
    int n, m;
    std::cin >> n;
    std::cin >> m;
    int *A = new int[n];
    int *B = new int[m];
    for (int i = 0; i < n; ++i) {
        std::cin >> A[i];
    }
    for (int i = 0; i < m; ++i) {
        std::cin >> B[i];
    }
    OverlapOfArrays(n, m, A, B);
    delete[] A;
    delete[] B;
    return 0;
}