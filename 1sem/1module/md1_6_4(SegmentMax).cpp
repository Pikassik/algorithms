//Скользящий максимум
//Дан массив целых чисел A[0..n), n не превосходит 100 000. Так же задан размер некотрого окна
// (последовательно расположенных элементов массива) в этом массиве k, k<=n. Требуется для каждого положения
// окна (от 0 и до n-k) вывести значение максимума в окне. Скорость работы O(n log n), память O(n).
#include <iostream>

class Heap {
public:
    Heap(const int *array, int n, int k)
    :buffersize_(std::max(n, 128))
    ,realsize_(k)
    {
        buffer_ = new int[k];
        position_ = new int[n];
        value_ = new int[n];
        for(int i = 0; i < n; ++i) { //создание константного буфера, содержащего элементы исходного массива
            value_[i] = array[i];
        }
        for(int i = 0; i < n; ++i) { //создание массива, хранящего индексы в куче, в которых хранятся индексы на
            position_[i] = i;        //элементы массива
        }
        for(int i = 0; i < k; ++i) { //непосредственно куча, в которой лежат индексы элементов массива, значениями
            buffer_[i] = i;          //которых она оперирует
        }
        BuildHeap();
    }
    ~Heap() {
        delete[] buffer_;
        delete[] position_;
        delete[] value_;
    }
    void SiftUp(int i) {
        while(true) {
            if ((value_[buffer_[i]] > value_[buffer_[(i - 1) / 2]])&&(i != 0)) {
                std::swap(buffer_[i], buffer_[(i - 1) / 2]);
                std::swap(position_[buffer_[i]], position_[buffer_[(i - 1) / 2]]);
                i = (i - 1) / 2;
            } else {
                break;
            }
        }
    }
    void  SiftDown(int i) {
        while(true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if ((left < realsize_) && (value_[buffer_[left]] > value_[buffer_[largest]])) {
                largest = left;
            }
            if ((right < realsize_) && (value_[buffer_[right]] > value_[buffer_[largest]])) {
                largest = right;
            }
            if (largest != i) {
                std::swap(buffer_[largest], buffer_[i]);
                std::swap(position_[buffer_[largest]], position_[buffer_[i]]);
                i = largest;
            } else {
                break;
            }
        }
    }
    int GetTop() {
        return value_[buffer_[0]];
    }
    int *buffer_;
    int *position_;
private:
    int buffersize_;
    int realsize_;
    int *value_;
    void BuildHeap() {
        for(int i = realsize_ / 2 - 1; i >= 0; --i) {
            SiftDown(i);
        }
    }
};

void FlexMaximum(int n, int k, const int *array, int *result) {
    Heap heap(array, n, k);
    for(int i = 0; i < n - k + 1; ++i) {
        result[i] = heap.GetTop();                     //считывание максимума кучи в результат
        if (i != n - k) {
            heap.buffer_[heap.position_[i]] = k + i;   //замена индекса i элемента на k + i и просеивание его
            heap.position_[k + i] = heap.position_[i]; //свое место
            heap.SiftUp(heap.position_[i]);
            heap.SiftDown(heap.position_[i]);
        }
    }

}

int main() {
    int n, k, *array;
    std::cin >> n;
    array = new int[n];
    for(int i = 0; i < n; ++i) {
        std::cin >> array[i];
    }
    std::cin >> k;
    int *result = new int[n - k + 1];
    FlexMaximum(n, k, array, result);
    for(int i = 0; i < n - k + 1; ++i) {
        std::cout << result[i] << ' ';
    }
    delete[] array;
    return 0;
}