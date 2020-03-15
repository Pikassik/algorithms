//Задача 6.
// Дан массив целых чисел в диапазоне [0..109]. Размер массива кратен 10 и ограничен
// сверху значением 2.5 * 107 элементов. Все значения массива являются элементами
// псевдо-рандомной последовательности. Необходимо отсортировать элементы массива
// за минимально время и вывести каждый десятый элемент отсортированной последовательности.
// Реализуйте сортировку, основанную на QuickSort.
// Минимальный набор оптимизаций, который необходимо реализовать:
// 1. Оптимизация ввода/вывода
// 2. Оптимизация выбора опорного элемента
// 3. Оптимизация Partition
// 4. Оптимизация рекурсии
// 5. Оптимизация концевой рекурсии
// Время работы O(nlogn) в среднем, Доп. память O(logn)
#include <iostream>
#include <vector>

using std::vector;
using std::swap;
using std::cin;
using std::cout;

void QSort(vector<int>&);

void Partition(vector<int>&, int, int, int&, int&);

int Median(vector<int>&, int, int);

void InsertionSort(vector<int> &array, int, int);

struct Segment {                 // небольшая структура, содержащая индексы начала и конца сортируемых отрезков для
    Segment(int begin, int end); // помещения их в стек.
    int begin;
    int end;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
    std::cin.tie(nullptr);                 // оптимизация ввода / вывода
    std::ios_base::sync_with_stdio(false);
    vector<int> inputarray;
    while (true) {
        int input;
        cin >> input;
        if (cin.fail()) {
            break;
        }
        inputarray.push_back(input);
    }
    QSort(inputarray);
    for (int i = 9; i < static_cast<int>(inputarray.size()); i += 10) {
        cout << inputarray[i] << ' ';
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void QSort(vector<int> &array) {
    vector<Segment> stack;               // стек для избавления от рекурсии
    stack.emplace_back(0, array.size()); // push первой итерации
    while (true) {
        int i, j;
        if (!stack.empty()) {
            if (stack.back().end - stack.back().begin < 32) { // маленькие подмассивы сортируются вставками
                InsertionSort(array, stack.back().begin, stack.back().end);
                stack.pop_back();
                continue;
            } else {
                Partition(array, stack.back().begin, stack.back().end, i, j);
            }
        } else {
            return;
        }
        int copy_begin = stack.back().begin;
        int copy_end = stack.back().end;
        stack.pop_back();
        if (i - copy_begin > copy_end - j) { // первым в стек кладется больший интервал (оптимизация концевой рекурсии)
            stack.emplace_back(copy_begin, i);
            stack.emplace_back(j, copy_end);
        } else {
            stack.emplace_back(j, copy_end);
            stack.emplace_back(copy_begin, i);
        }
    }
}


// Данная реализация Partition разделяет массив на 3 интервала :
// [begin, i) меньше опорного, [i, j) равны опорному, [j, end) больше опорного.
void Partition(vector<int> &array, int begin, int end, int &i, int &j) {
    swap(array[Median(array, begin, end)], array[end - 1]); //как опорный элемент выбирается медиана из первого,
    const int pivot = array[end - 1];                       //серединного и последнего элементов
    --end;
    int border = begin;
    while (border <= end) {
        if (array[border] < pivot) {
            swap(array[begin], array[border]);
            ++begin;
            ++border;
        } else if (array[border] == pivot) {
            ++border;
        } else {
            swap(array[border], array[end]);
            --end;
        }
    }
    i = begin;
    j = border;
}

int Median(vector<int> &array, int begin, int end) {
    int middle = end - begin / 2;
    if ((array[begin] <= array[middle]) && (array[middle] <= array[end - 1])) {
        return middle;
    } else if ((array[middle] <= array[begin]) && (array[begin] <= array[end - 1])) {
        return begin;
    } else {
        return end - 1;
    }
}

void InsertionSort(vector<int> &array, int begin, int end) {
    for (int i = begin; i < end; ++i) {
        int copy = array[i];
        for(int j = i - 1; (j >= begin) && copy < array[j]; --j) {
            swap(array[j], array[j + 1]);
        }
    }
}

Segment::Segment(int begin, int end)
:begin(begin)
,end(end)
{}