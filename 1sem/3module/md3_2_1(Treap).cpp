//2_1. Декартово дерево. Разница глубин
//Дано число N < 106 и последовательность пар целых чисел из [-231, 231] длиной N.
//Построить декартово дерево из N узлов, характеризующихся парами чисел (Xi, Yi).
//Каждая пара чисел (Xi, Yi) определяет ключ Xi и приоритет Yi в декартовом дереве.
//Добавление узла в декартово дерево выполняйте следующим образом: При добавлении узла (x, y) выполняйте спуск
//по ключу до узла P с меньшим приоритетом. Затем разбейте найденное поддерево по ключу x так,
//чтобы в первом поддереве все ключи меньше x, а во втором больше или равны x. Получившиеся два дерева сделайте
//дочерними для нового узла (x, y). Новый узел вставьте на место узла P.
//Построить также наивное дерево поиска по ключам Xi.
//Т.е., при добавлении очередного числа K в наивное дерево с корнем root, если root→Key ≤ K,
//то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
//Вычислить разницу глубин наивного дерева поиска и декартового дерева. Разница может быть отрицательна,
//необходимо вывести модуль разности.
//Время работы O(n^2) в худшем случае, в среднем O(nlogn), доп. память O(1)

#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::max;
using std::abs;
using std::make_pair;

template <class T, class U, class TCmp = std::less<T>, class UCmp = std::less<U>>
class Treap {
public:
    explicit Treap(const TCmp &tcompare = TCmp(), const UCmp &ucompare = UCmp());
    ~Treap();
    void Insert(const T &key, const U &priority);
    int GetHeight();
private:
    struct Node {
        T key;
        U priority;
        Node *left;
        Node *right;
        explicit Node(const T &key, const U &priority);
    };
    Node *root_;
    int size_;
    TCmp tcmp_;
    UCmp ucmp_;
    int Height(Node *node);
    pair<Node*, Node*> Split(Node *tree, const T &key);
};

template <class T, class Cmp = std::less<>>
class BST {
public:
    explicit BST(const Cmp &compare = Cmp());
    ~BST();
    void Insert(const T &key);
    int GetHeight();
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
    int Height(Node *node);
};

///////////////////////////////////////////////////////////////////////////////

int main() {
    int n;
    cin >> n;
    BST<int> default_tree;
    Treap<int, int> treap_tree;
    for (int i = 0; i < n; ++i) {
        int input_key, input_priority;
        cin >> input_key >> input_priority;
        default_tree.Insert(input_key);
        treap_tree.Insert(input_key, input_priority);
    }
    cout << abs(default_tree.GetHeight() - treap_tree.GetHeight());
}

///////////////////////////////////////////////////////////////////////////////

template <class T, class U, class TCmp, class UCmp>
Treap<T, U, TCmp, UCmp>::Treap(const TCmp &tcompare, const UCmp &ucompare)
    :root_(nullptr)
    ,size_(0)
    ,tcmp_(tcompare)
    ,ucmp_(ucompare) {}

template <class T, class U, class TCmp, class UCmp>
Treap<T, U, TCmp, UCmp>::~Treap() {
    vector<Node*> stack;
    if (root_ != nullptr) {
        stack.push_back(root_);
    } else {
        return;
    }
    while (!stack.empty()) {
        if (stack.back()->left != nullptr) {
            Node *tmp = stack.back()->left;
            stack.back()->left = nullptr;
            stack.push_back(tmp);
        } else if (stack.back()->right != nullptr) {
            Node *tmp = stack.back()->right;
            stack.back()->right = nullptr;
            stack.push_back(tmp);
        } else {
            delete stack.back();
            stack.pop_back();
        }
    }
}

template <class T, class U, class TCmp, class UCmp>
void Treap<T, U, TCmp, UCmp>::Insert(const T &key, const U &priority) {
    ++size_;
    if (root_ == nullptr) {                                             //Нет корня -> создаем
        root_ = new Node(key, priority);
        return;
    }
    Node *previous = nullptr;                                           //Храним при спуске родителя
    Node *current = root_;
    while ((current != nullptr) && (!ucmp_(current->priority, priority))) {   //Спускаемся по ключу, пока не наткнемся
        previous = current;                                             //на ноду с приоритетом ниже, либо на нуль
        if (tcmp_(key, current->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    pair<Node*, Node*> children = Split(current, key);                  //Разбиваем поддерево по ключу и делаем
    Node *new_node = new Node(key, priority);                           //поддеревья детьми новой ноды
    new_node->left = children.first;
    new_node->right = children.second;
    if (previous != nullptr) {                                          //Если есть родитель, то привязываем его,
        if (tcmp_(key, previous->key)) {                                 //если нет, то мы разбивали дерево и
            previous->left = new_node;                                  //перепривязваем корень на новую ноду
        } else {
            previous->right = new_node;
        }
    } else {
        root_ = new_node;
    }
}

template <class T, class U, class TCmp, class UCmp>
int Treap<T, U, TCmp, UCmp>::GetHeight() {
    return Height(root_);
}

template <class T, class U, class TCmp, class UCmp>
Treap<T, U, TCmp, UCmp>::Node::Node(const T &key, const U &priority)
    :key(key)
    ,priority(priority)
    ,left(nullptr)
    ,right(nullptr) {}

template <class T, class U, class TCmp, class UCmp>
int Treap<T, U, TCmp, UCmp>::Height(Node *node) {                                 //рекурсивный поиск высоты за O(n)
    if (node == nullptr) {
        return 0;
    }
    return max(Height(node->left), Height(node->right)) + 1;
}

template <class T, class U, class TCmp, class UCmp>
pair<class Treap<T, U, TCmp, UCmp>::Node*, class Treap<T, U, TCmp, UCmp>::Node*>
                               Treap<T, U, TCmp,  UCmp>::Split(Node *tree, const T &key) {
    if (tree == nullptr) {                                              //Обычный рекурсивный Split декартового дерева
        return make_pair(nullptr, nullptr);
    } else if (tcmp_(key, tree->key)) {
        pair<Node*, Node*> next = Split(tree->left, key);
        tree->left = next.second;
        next.second = tree;
        return next;
    } else {
        pair<Node*, Node*> next = Split(tree->right, key);
        tree->right = next.first;
        next.first = tree;
        return next;
    }
}

///////////////////////////////////////////////////////////////////////////////


template <class T, class Cmp>
BST<T, Cmp>::BST(const Cmp &compare)
    :root_(nullptr)
    ,size_(0)
    ,cmp_(compare) {}

template <class T, class Cmp>
BST<T, Cmp>::~BST() {                           //Деструктор представляет собой DFS post-order, удаляющий листья
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
int BST<T, Cmp>::GetHeight() {
    return Height(root_);
}

template <class T, class Cmp>
BST<T, Cmp>::Node::Node(const T &key)
    :key(key)
    ,left(nullptr)
    ,right(nullptr) {}

template <class T, class Cmp>
int BST<T, Cmp>::Height(Node *node) {
    if (node == nullptr) {
        return 0;
    }
    return max(Height(node->left), Height(node->right)) + 1;
}