#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::swap;
using std::endl;
using std::vector;
using std::pair;

template <class T, class Cmp = std::less<>>
class BST {
public:
    BST(const Cmp &cmp = Cmp());
    ~BST();
    int Size();
    void Insert(T key);
    bool Erase(T key);
    bool Search(T key);
    pair<bool, T> Next(T key);
    pair<bool, T> Prev(T key);
    vector<T> TraverseDFSPostOrder();
private:
    struct Node {
        T key;
        Node *left;
        Node *right;
        Node *parent;
        bool flag;
        Node(T key);
    };
    Node *root_;
    int size_;
    Cmp cmp_;
    pair<bool, Node*> Find(T key);
    void DeleteNode(Node *&node);
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
    vector<int> outputarray;
    outputarray = tree.TraverseDFSPostOrder();
    for (auto output: outputarray) {
        cout << output << ' ';
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

template <class T, class Cmp>
BST<T, Cmp>::BST(const Cmp &cmp)
:root_(nullptr)
,size_(0)
,cmp_(cmp)
{}

template <class T, class Cmp>
BST<T, Cmp>::~BST() {
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

template <class T, class Cmp>
int BST<T, Cmp>::Size() {
    return size_;
}

template <class T, class Cmp>
void BST<T, Cmp>::Insert(T key) {
    Node *current = root_;
    Node *previous = nullptr;
    if (root_ == nullptr) {
        root_ = new Node(key);
        ++size_;
        return;
    }
    while (true) {
        if (current == nullptr) {
            if (cmp_(key, previous->key)) {
                previous->left = new Node(key);
                previous->left->parent = previous;
            } else {
                previous->right = new Node(key);
                previous->right->parent = previous;
            }
            ++size_;
            return;
        } else if (cmp_(key, current->key)) {
            previous = current;
            current = current->left;
        } else {
            previous = current;
            current = current->right;
        }
    }
}

template <class T, class Cmp>
bool BST<T, Cmp>::Erase(T key) {
    pair<bool, Node*> node = Find(key);
    if (node.first) {
        DeleteNode(node.second);
        --size_;
        return true;
    } else {
        return false;
    }
}

template <class T, class Cmp>
bool BST<T, Cmp>::Search(T key) {
    return Find(key).first;
}

template <class T, class Cmp>
pair<bool, T> BST<T, Cmp>::Next(T key) {
    pair<bool, Node*> node = Find(key);
    Node *current = node.second;
    if (node.first) {
        if (current->right != nullptr) {
            current = current->right;
            while (current->left != nullptr) {
                current = current->left;
            }
            return std::make_pair(true, current->key);
        }
        while (current->parent != nullptr) {
            if (current->parent->left == current) {
                return std::make_pair(true, current->parent->key);
            }
            current = current->parent;
        }
    }
    return std::make_pair(false, key);
};

template <class T, class Cmp>
pair<bool, T> BST<T, Cmp>::Prev(T key) {
    pair<bool, Node*> node = Find(key);
    Node *current = node.second;
    if (node.first) {
        if (current->left_ != nullptr) {
            current = current->left;
            while (current->right != nullptr) {
                current = current->right;
            }
            return std::make_pair(true, current->key);
        }
        while (current->parent != nullptr) {
            if (current->parent->right == current) {
                return std::make_pair(true, current->parent->key);
            }
            current = current->parent;
        }
    }
    return std::make_pair(false, key);
};

template <class T, class Cmp>
vector<T> BST<T, Cmp>::TraverseDFSPostOrder() {
    vector<T> array;
    array.reserve(size_);
    vector<Node*> stack;
    if (root_ != nullptr) {
        stack.push_back(root_);
    } else {
        return array;
    }
    while (!stack.empty()) {
        if ((stack.back()->left != nullptr) && !(stack.back()->left->flag)) {
            stack.push_back(stack.back()->left);
        } else if ((stack.back()->right != nullptr) && !(stack.back()->right->flag)) {
            stack.push_back(stack.back()->right);
        } else {
            array.push_back(stack.back()->key);
            if (stack.back()->left != nullptr) {
                stack.back()->left->flag = false;
            }
            if (stack.back()->right != nullptr) {
                stack.back()->right->flag = false;
            }
            stack.back()->flag = true;
            stack.pop_back();
        }
    }
    root_->flag = false;
    return array;
}

template <class T, class Cmp>
BST<T, Cmp>::Node::Node(T key)
:key(key)
,parent(nullptr)
,left(nullptr)
,right(nullptr)
,flag(false)
{}

template <class T, class Cmp>
pair<bool, class BST<T, Cmp>::Node*> BST<T, Cmp>::Find(T key) {
    Node *current = root_;
    while (true) {
        if (current == nullptr) {
            return std::make_pair(false, nullptr);
        } else if (!(cmp_(current->key, key)) && !(cmp_(key, current->key))) {
            return std::make_pair(true, current);
        } else if (cmp_(key, current->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
}

template <class T, class Cmp>
void BST<T, Cmp>::DeleteNode(Node *&node) {
    if (node->left == nullptr) {
        Node *right = node->right;
        delete node;
        node = right;
    } else if (node->right == nullptr) {
        Node *left = node->left;
        delete node;
        node = left;
    } else {
        Node *minparent = node;
        Node *min = node->right;
        while(min->left != nullptr) {
            minparent = min;
            min = min->left;
        }
        if (minparent->left == min) {
            minparent->left = min->right;
        } else {
            minparent->right = min->right;
        }
        node->key = min->key;
        delete min;
    }
}