//1_3. Обход дерева в порядке post-order
//Дано число N ≤ 104 и последовательность целых чисел из [-231..231] длиной N. Требуется построить бинарное дерево,
//заданное наивным порядком вставки. Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
//то узел K добавляется в правое поддерево root; иначе в левое поддерево root. Выведите элементы в порядке
//post-order (снизу вверх).
//Рекурсия запрещена.
//Время работы O(n^2), доп. память O(n)

#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::swap;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;

template <class T, class Cmp = std::less<>>
class BST {
public:
    explicit BST(const Cmp &compare = Cmp());
    ~BST();
    void Insert(const T &key);
    vector<T> TraverseDFSPostOrder() const;
    vector<T> InOrder();
private:
    struct Node {
        T key;
        Node *left;
        Node *right;
        explicit Node(const T &key);
    };
    Node *root_;
    int size_;
    Cmp cmp_;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
    int n;
    cin >> n;
    BST<int> tree;
    for (int i = 0; i < n; ++i) {
        int input;
        cin >> input;
        tree.Insert(input);
    }
    for (auto output: tree.InOrder()) { //Вызов DFS и вывод содержимого вектора
        cout << output << ' ';
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

template <class T, class Cmp>
BST<T, Cmp>::BST(const Cmp &compare)
    :root_(nullptr)
    ,size_(0)
    ,cmp_(compare) {}

template <class T, class Cmp>
BST<T, Cmp>::~BST() { //Деструктор представляет собой DFS post-order, удаляющий листья
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

template <class T, class Cmp>
void BST<T, Cmp>::Insert(const T &key) {
    Node *current = root_;                      //храним текущую ноду и предыдущую при спуске
    Node *previous = nullptr;
    if (root_ == nullptr) {                     //если корня нет, создаем его
        root_ = new Node(key);
        ++size_;
        return;
    }
    while (true) {
        if (current == nullptr) {               //как только попали в нуль, создаем новую ноду
            if (cmp_(key, previous->key)) {     //привязываем предыдущую к новой
                previous->left = new Node(key);
            } else {
                previous->right = new Node(key);
            }
            ++size_;
            return;
        } else if (cmp_(key, current->key)) {   //идем в левое поддерево, если ключ < корня
            previous = current;
            current = current->left;
        } else {                                //идем в правое поддерево, если ключ >= корня
            previous = current;
            current = current->right;
        }
    }
}

template <class T, class Cmp>
vector<T> BST<T, Cmp>::TraverseDFSPostOrder() const {
    vector<T> array;
    array.reserve(size_);
    vector<pair<Node*, bool>> stack;
    if (root_ != nullptr) {
        stack.push_back(make_pair(root_, false));
    } else {
        return array;
    }
    while (!stack.empty()) {
        if (!stack.back().second) {                         //Если еще не были в вершине, то кладем ее детей
            stack.back().second = true;
            Node *left_child = stack.back().first->left;
            Node *right_child = stack.back().first->right;
            if (right_child != nullptr) {                   //Если есть правый/левый ребенок, то кладем его в стекы
                stack.push_back(make_pair(right_child, false));
            }
            if (left_child != nullptr) {
                stack.push_back(make_pair(left_child, false));
            }
        } else {                                            //Все дети посещены либо их нет
            array.push_back(stack.back().first->key);       //Кладем в стек ключ текущей ноды
            stack.pop_back();                               //Делаем pop ноды
        }
    }
    return array;
}

template <class T, class Cmp>
BST<T, Cmp>::Node::Node(const T &key)
    :key(key)
    ,left(nullptr)
    ,right(nullptr) {}