//2.3 Закраска прямой.
//На числовой прямой окрасили N отрезков.
//Известны координаты левого и правого концов каждого отрезка [Li, Ri]. Найти длину окрашенной части числовой прямой.
//N ≤ 10000. Li, Ri — целые числа в диапазоне [0, 109].
//Время работы O(nlog(n)) Доп. память O(1)
#include <iostream>
#include <vector>
#include <algorithm>

using std::swap;
using std::endl;
using std::cin;
using std::cout;
using std::vector;

struct Segment {
    int left_;
    int right_;
    Segment(int, int);
};

bool operator< (const Segment&, const Segment&);

class Heap {
public:
    Heap(vector<Segment> *array);
    void Pop();
private:
    void SiftDown(int);
    void BuildHeap();
    vector<Segment> *buffer_;
    int buffersize_;
};

void HeapSort(vector<Segment>&);

int LineFill(vector<Segment> &array);

///////////////////////////////////////////////////////////////////////////////
int main() {
    vector<Segment> inputarray;
    int n;
    cin >> n;
    inputarray.reserve(n);
    for (int i = 0; i < n; ++i) {
        int left, right;
        cin >> left >> right;
        inputarray.emplace_back(left, right);
    }
    cout << LineFill(inputarray);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

Segment::Segment(int left, int right)
:left_(left)
,right_(right)
{}

bool operator< (const Segment &element1, const Segment &element2) {
    if (element1.left_ < element2.left_) {        //сравниваем по левой границе, если они равны, то по правой.
        return  true;
    } else if (element1.left_ > element2.left_) {
        return false;
    } else {
        return (element1.right_ < element2.right_);
    }
}

Heap::Heap(vector<Segment> *array)
:buffersize_(static_cast<int>((*array).size()))
{
    buffer_ = array;
    BuildHeap();
}

void Heap::Pop() {
    swap((*buffer_)[0], (*buffer_)[buffersize_ - 1]);
    --buffersize_;
    SiftDown(0);
}

void Heap::SiftDown(int i) {
    while(true) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if ((left < buffersize_) && ((*buffer_)[largest] < (*buffer_)[left])) {
            largest = left;
        }
        if ((right < buffersize_) && ((*buffer_)[largest] < (*buffer_)[right])) {
            largest = right;
        }
        if (largest != i) {
            swap((*buffer_)[largest], (*buffer_)[i]);
            i = largest;
        } else {
            break;
        }
    }
}

void Heap::BuildHeap() {
    for (int i = buffersize_ / 2 - 1; i >= 0; --i) {
        SiftDown(i);
    }
}

void HeapSort(vector<Segment> &array) {
    Heap heap(&array);
    for (int i = 0; i < static_cast<int>(array.size()); ++i) {
        heap.Pop();
    }
}

int LineFill(vector<Segment> &array) {
    HeapSort(array);                            //сортируем массив отрезков, используя модифицированный оператор меньше
    int length = 0;
    for (int i = 0; i < static_cast<int>(array.size()); ++i) {    //проходимся по всем отрезкам
        Segment current = array[i];
        int right_border = current.right_;
        while ((i + 1 < static_cast<int>(array.size())) && (array[i + 1].left_ <= right_border)) {
            ++i;                                 //пока следующий отрезок не вылезает за границу предыдущего,
            if (array[i].right_ > right_border) {//перемещаем правый край
            right_border = array[i].right_;
            }
        }
        length += right_border - current.left_;  //если отрезок вылез за границу или они кончились, то добавляем к
    }                                            //итоговой длине разность краев.
    return length;
}