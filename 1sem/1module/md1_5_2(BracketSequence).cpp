#include <iostream>
#include <cstdio>
#include <cstring>

using std::cin;
using std::cout;
using std::endl;

class Stack {
public:
    Stack()
    :top_(0)
    ,realsize_(0)
    ,buffersize_(128) {
        buffer_ = new char[buffersize_];
    }
    ~Stack() {
        delete[] buffer_;
    }
    void Push(char symbol) {
        ++realsize_;
        if (realsize_ > buffersize_) {
            Grow();
        }
        ++top_;
        buffer_[top_] = symbol;
    }
    char Pop(){
        if (realsize_ == 0) {
            return -1;
        }
        char result = buffer_[top_];
        --top_;
        --realsize_;
        return result;
    }
    char GetTop() {
        if (realsize_ == 0) {
            return -1;
        } else {
            return buffer_[top_];
        }
    }
private:
    int buffersize_;
    char *buffer_;
    int realsize_;
    int top_;
    void Grow() {
        char *newbuffer = new char[buffersize_ * 2];
        for(int i = 0; i < realsize_; ++i) {
            newbuffer[i] = buffer_[i];
        }
        top_ = realsize_ - 1;
        buffersize_ *= 2;
        delete[] buffer_;
        buffer_ = newbuffer;
    }

};

int main() {
    Stack stack;
    char string1[100001];
    char string2[100001];
    scanf("%s", string1);
    scanf("%s", string2);
    int counter = 0;
    for(int i = 0; string1[i] != 0; ++i) {
        stack.Push(string1[i]);
        while(string2[counter] == stack.GetTop()) {
            stack.Pop();
            ++counter;
        }
    }
    if (counter == strlen(string1)) {
        cout << "YES" << endl;
    } else {
        cout << "NO" << endl;
    }
    return 0;
}