//Задача 4. Алгоритм сжатия данных Хаффмана
//Напишите две функции для создания архива из одного файла и извлечения файла из архива.
//Время работы O(n * k), Доп. память O(k), где н - кол-во байт в потоке, k - размер алфавита
#include "Huffman.h"
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using std::vector;
using std::queue;
using std::stack;
using std::reverse;

void Encode(IInputStream &original, IOutputStream &compressed);

void Decode(IInputStream &compressed, IOutputStream &original);

class HTree {
public:
    HTree(IInputStream &input_stream, IOutputStream &output_stream);
    ~HTree();
    void Encode();
    void Decode();
private:
    struct Node {
        Node *left;
        Node *right;
        Node *parent;
        int weight;
        byte symbol;
        Node();
        explicit Node(byte symbol);
    };
    void RepeatSymbol();                            //Вызывается при повторном считывании символа при кодировании
    void AddSymbol();                               //Вызывается при первом считывании символа при кодировании
    void Repair(Node *node);                        //Функция упорядочивания дерева
    void Swap(Node *broken, Node *swapped);         //Функция обмена вершин
    inline byte ReadBit();                          //Функция побитового считывания из потока
    void WriteBit(byte bit);                        //Функция побитовой записи в поток
    inline bool IsHere() const;                     //Функция проверки на наличие символа в дереве
    IInputStream &input_stream_;
    IOutputStream &output_stream_;
    byte input_byte_;
    byte output_byte_;
    byte counter_;
    vector<Node*> symbols_;                         //Массив указателей на соответствующие символам вершины
    Node *root_;
    Node *escape_;
};

///////////////////////////////////////////////////////////////////////////////
////Функции Кодирования и декодирования////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Encode(IInputStream &original, IOutputStream &compressed) {
    HTree tree(original, compressed);
    tree.Encode();
}

void Decode(IInputStream &compressed, IOutputStream &original) {
    HTree tree(compressed, original);
    tree.Decode();
}

///////////////////////////////////////////////////////////////////////////////
////Реализация класса HTree////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HTree::HTree(IInputStream &input_stream, IOutputStream &output_stream)
:input_stream_(input_stream)
,output_stream_(output_stream)
,input_byte_(0)
,output_byte_(0)
,counter_(0) {
    escape_ = new Node;
    root_ = escape_;
    for (int i = 0; i < 256; ++i) {
        symbols_.push_back(nullptr);
    }
}

HTree::~HTree() { //Деструктор представляет собой DFS post-order, удаляющий листья
    vector<Node*> stack;
    if (root_ != nullptr) {
        stack.push_back(root_);
    } else {
        return;
    }
    while (!stack.empty()) {
        if (stack.back()->left != nullptr) {    //Кладем в стек левого ребенка, если есть
            Node *tmp = stack.back()->left;
            stack.back()->left = nullptr;       //Ставим метку, обнуляя указатель, чтобы при повторном посещении
            stack.push_back(tmp);               //не положить несуществующий node в стек
        } else if (stack.back()->right != nullptr) { // --//--
            Node *tmp = stack.back()->right;
            stack.back()->right = nullptr;
            stack.push_back(tmp);
        } else {
            delete stack.back();                //Если нет детей, то удаляем node
            stack.pop_back();
        }
    }
}

void HTree::Encode() {
    while(input_stream_.Read(input_byte_)) {
        if (IsHere()) {
            RepeatSymbol();
        } else {
            AddSymbol();
        }
    }
    vector<byte> stack;
    stack.push_back(static_cast<byte>(1));          //Кладем лишнюю единичку, чтобы определить конец считывания.
    Node *current = escape_;                        //Выводим путь от корня до escape
    while (current->parent != nullptr) {
        if (current->parent->left == current) {
            stack.push_back(static_cast<byte>(0));
        } else {
            stack.push_back(static_cast<byte>(1));
        }
        current = current->parent;
    }
    while (!stack.empty()) {
        WriteBit(stack.back());
        stack.pop_back();
    }
    if (counter_ != 0) {                            //Если последний байт записался не до конца, то выводим его
        output_stream_.Write(output_byte_ << (8 - counter_));
    }
}

void HTree::Decode() {
    while (true) {
        Node *current;
        current = root_;                            //Спуск по дереву
        while ((current->left != nullptr) && (current->right != nullptr)) {
            if (ReadBit() == static_cast<byte>(1)) {
                current = current->right;
            } else {
                current = current->left;
            }
        }
        if (current->weight == 0) {                 //Попали в escape
            if (ReadBit() == 1) {                   //Если конец архива, то выход из функции.
                return;
            } else {                                //Не конец архива -> выводим байт из потока
                output_byte_ = 0;
                for (int i = 0; i < 8; ++i) {
                    output_byte_ <<= 1;
                    output_byte_ += ReadBit();
                }
                output_stream_.Write(output_byte_);
                current->left = new Node;           //Создаем новый escape
                current->right = new Node(output_byte_); //Создаем новый лист со значением, считанным из потока
                current->left->parent = current;    //Перепривязываем указатели
                current->right->parent = current;
                Node *tmp = current;
                current = current->right;           //Увеличиваем вес родителей
                while (current->parent != nullptr) {
                    ++current->parent->weight;
                    current = current->parent;
                }
                symbols_[output_byte_] = tmp->right;//Сохраняем в массив указатель на лист с данным символом
                escape_ = tmp->left;
                Repair(escape_);                    //Упорядочиваем дерево
            }
        } else {                                    //Попали в лист, содержащий символ
            ++current->weight;                      //Обновляем вес
            Node *repaired = current;
            output_stream_.Write(repaired->symbol); //Выводим символ из этого листа
            while (current->parent != nullptr) {    //Обновляем веса родителей
                ++current->parent->weight;
                current = current->parent;
            }
            Repair(repaired);                       //Упорядочиваем дерево
        }
    }
}

HTree::Node::Node()
    :left(nullptr)
    ,right(nullptr)
    ,parent(nullptr)
    ,weight(0)
    ,symbol(0) {}

HTree::Node::Node(byte symbol)
    :left(nullptr)
    ,right(nullptr)
    ,parent(nullptr)
    ,weight(1)
    ,symbol(symbol) {}

void HTree::RepeatSymbol() {
    ++symbols_[input_byte_]->weight;                //Обновляем вес листа
    Node *current = symbols_[input_byte_];          //Выводим путь от корня к листу(Поднимаемся до корня и кладем в стек
    vector<byte> stack;                             //путь)
    while (current->parent != nullptr) {
        ++current->parent->weight;
        if (current->parent->left == current) {
            stack.push_back(static_cast<byte>(0));
        } else {
            stack.push_back(static_cast<byte>(1));
        }
        current = current->parent;
    }
    while (!stack.empty()) {
        WriteBit(stack.back());
        stack.pop_back();
    }
    Repair(symbols_[input_byte_]);                  //Упорядочиваем дерево
}

void HTree::AddSymbol() {
    escape_->left = new Node;                       //Создание нового escape
    escape_->right = new Node(input_byte_);         //Создание нового листа для нового символа
    symbols_[input_byte_] = escape_->right;         //Обновление-перепривязка указателей
    escape_->left->parent = escape_;
    escape_->right->parent = escape_;
    ++escape_->weight;                              //Обновляем вес вершины
    vector<byte> stack;
    Node *current = escape_;
    stack.push_back(static_cast<byte>(0));          //Кладем лишний нуль, чтобы определить конец передачи
    while (current->parent != nullptr) {            //Выводим путь к escape(Поднимаемся до корня и кладем в стек путь)
        if (current->parent->left == current) {
            stack.push_back(static_cast<byte>(0));
        } else {
            stack.push_back(static_cast<byte>(1));
        }
        ++current->parent->weight;
        current = current->parent;
    }
    while (!stack.empty()) {
        WriteBit(stack.back());
        stack.pop_back();
    }
    for (int i = 7; i >= 0; --i) {                  //Выводим символ
        WriteBit((input_byte_ >> i) % static_cast<byte>(2));
    }
    escape_ = escape_->left;                        //Обновляем позицию escape
    Repair(escape_);                                //Упорядочиваем дерево
}

void HTree::Repair(Node *node) {                    //Для того, чтобы обойти дерево в нужном порядке (снизу вверх слева
    queue<Node*> queue;                             //направо), делаем BFS дерева справа налево, кладя веса в стек,
    vector<Node*> stack;                            //после чего разворачиваем стек и проверяем дерево на
    queue.push(root_);                              //упорядоченность
    while (queue.front() != node) {
        stack.push_back(queue.front());
        if (queue.front()->right != nullptr) {
            queue.push(queue.front()->right);
        }
        if (queue.front()->left != nullptr) {
            queue.push(queue.front()->left);
        }
        queue.pop();
    }
    stack.push_back(node);
    reverse(stack.begin(), stack.end());            //Поиск такой вершины, что weight(a[i]) > weight(a[i + 1]),если есть
    for (int i = 0; i < static_cast<int>(stack.size()) - 1; ++i) {
        if (stack[i]->weight > stack[i + 1]->weight) {
            Node *broken = stack[i];
            Node *swapped = stack[i + 1];
            int j = i + 1;
            for( ; i >= 0; --i) {
                if (stack[i]->weight == broken->weight) {//Поиск вершины со "сломанным" весом с самым маленьким индексом
                    broken = stack[i];
                }
            }
            for ( ; j < static_cast<int>(stack.size()); ++j) {//Поиск вершины с весом меньше сломанного
                if (stack[j]->weight < broken->weight) {      //и максимальным индексом
                    swapped = stack[j];
                }
            }
            Swap(broken, swapped);                            //Исправление дерева путем обмена вершин
            break;
        }
    }
}

void HTree::Swap(Node *broken, Node *swapped) {
    Node *current = broken;
    while (current->parent != nullptr) {            //Уменьшаем вес родителей, т.к. weight(broken) = weight(swapped) + 1
        --current->parent->weight;
        current = current->parent;
    }
    Node *broken_parent = broken->parent;           //Перепривязка указателей
    Node *swapped_parent = swapped->parent;
    bool flag = (swapped_parent->left == swapped);  //Если родитель у обмениваемых вершин совпадает, то может произойти
    if (broken_parent->left == broken) {            //ошибка, поэтому нужно сохранить результат сравнения до изменения
        broken_parent->left = swapped;              //детей
    } else {
        broken_parent->right = swapped;
    }
    if (flag) {
        swapped_parent->left = broken;
    } else {
        swapped_parent->right = broken;
    }
    swapped->parent = broken_parent;
    broken->parent = swapped_parent;
    current = broken;                               //Повышаем веса родителей, т.к. weight(broken) = weight(swapped) + 1
    while (current->parent != nullptr) {
        ++current->parent->weight;
        current = current->parent;
    }
}

inline byte HTree::ReadBit() {                      //Функция побитового считывания
    if (counter_ == 0) {
        input_stream_.Read(input_byte_);
    }
    byte tmp = static_cast<byte>((input_byte_ >> (7 - counter_)) % static_cast<byte>(2));
    ++counter_;
    if (counter_ == 8) {
        counter_ = 0;
    }
    return tmp;
}

void HTree::WriteBit(byte bit) {                    //Функция побитовой записи
    output_byte_ <<= 1;
    output_byte_ += bit;
    ++counter_;
    if (counter_ == 8) {
        output_stream_.Write(output_byte_);
        output_byte_ = 0;
        counter_ = 0;
    }
}

inline bool HTree::IsHere() const {                 //Проверка на нахождение символа в дереве
    return (symbols_[input_byte_] != nullptr);
}