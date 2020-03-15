//Реализовать очередь с динамическим зацикленным буфером.
//Обрабатывать команды push back и pop front.
#include <iostream>

class DinamicQueue {
public:
    DinamicQueue()
    :buffersize_(1)
    ,tail_(0)
    ,head_(0)
    ,realsize_(0) {
        buffer_ = new int[1];
    }
    ~DinamicQueue() {
        delete[] buffer_;
    }
    void PushBack(int value) {
        ++realsize_;
        if (realsize_ > buffersize_) {                        //если буфер слишком мал, то создается новый, в 2 раза
            Grow();                                          //больше, с переносом старых элементов в новый
        }
        if (realsize_ != 1) {
            tail_ = (tail_ + 1) % buffersize_;
        }
        buffer_[tail_] = value;
    }
    int PopFront() {
        if (realsize_ == 0) {
            return -1;
        }
        int result = buffer_[head_];
        if (realsize_ != 1){
            head_ = (head_ + 1) % buffersize_;
        }
        --realsize_;
        if ((realsize_ < buffersize_ / 4)&&(realsize_ > 32)) {//если кол-во элементов сильно уменьшилось(в 4 и более раз
            BufferDecrease();                                 //относительно размера буфера), то создается новый буфер
        }                                                     //с переносом старых элементов в новый
        return result;
    }
private:
    int *buffer_;
    int buffersize_;
    int realsize_;
    int head_;
    int tail_;
    void Grow() {
        int *newbuffer = new int[buffersize_ * 2];
        int i;
        if (tail_ >= head_) {                                 //перенос элементов
            for (i = 0; i < realsize_ - 1; ++i) {
                newbuffer[i] = buffer_[head_ + i];
            }
        } else {
            for (i = 0; i < realsize_ - 1; ++i){
                newbuffer[i] = buffer_[(head_ + i) % buffersize_];
            }
        }
        tail_ = i - 1;
        buffersize_ *= 2;
        head_ = 0;
        delete[] buffer_;
        buffer_ = newbuffer;
    }
    void BufferDecrease() {
        int *newbuffer = new int[buffersize_ / 2];
        int i;
        if (tail_ >= head_) {                                 //перенос элементов
            for (i = 0; i < realsize_; ++i) {
                newbuffer[i] = buffer_[head_ + i];
            }
        } else {
            for (i = 0; i < realsize_; ++i) {
                newbuffer[i] = buffer_[(head_ + i) % buffersize_];
            }
        }
        tail_ = i - 1;
        buffersize_ /= 2;
        head_ = 0;
        delete[] buffer_;
        buffer_ = newbuffer;
    }
};

int main() { // тестирующая программа
    int n;
    DinamicQueue queue1;
    std::cin >> n;
    bool flag = true;
    for (int i = 0; i < n; ++i) {
        int a, b;
        std::cin >> a >> b;
        if (a == 2) {
            if (queue1.PopFront() != b) {
                flag = false;
            }
        } else {
            queue1.PushBack(b);
        }
    }
    if (flag) {
        std::cout << "YES";
    } else {
        std::cout << "NO";
    }
    return 0;
}