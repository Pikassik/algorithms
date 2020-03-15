//3_1. Солдаты
//В одной военной части решили построить в одну шеренгу по росту. Т.к. часть была далеко не образцовая, то солдаты
// часто приходили не вовремя, а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
// Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту – сначала самые высокие,
// а в конце – самые низкие. За расстановку солдат отвечал прапорщик, который заметил интересную особенность – все
// солдаты в части разного роста. Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
// а именно для каждого приходящего солдата указывать, перед каким солдатом в строе он должен становится.
// Требуемая скорость выполнения команды - O(log n).Время работы команды - O(logn), Доп. память O(logn)(для рекурсии)
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::max;
using std::make_pair;

class SoldierCompare {
 public:
  bool operator()(int left, int right) const;
};

template<class T, class Cmp = std::less<>>
class BST {
 public:

  explicit BST(const Cmp &compare = Cmp());
  virtual ~BST();
  virtual void Insert(const T &key);
  virtual bool Erase(const T &key);
  bool Search(const T &key);

 protected:

  struct Node {
    T key;
    Node *left;
    Node *right;
    char height;
    int weight;            //Кол-во вершин, содержащихся в поддереве данной ноды
    explicit Node(const T &key);
  };

  Node *root_;
  int size_;
  Cmp cmp_;

  Node *FindMin(Node *root);

 private:

  pair<bool, Node *> Find(const T &key);

  void DeleteNode(Node *&node);

};

template<class T, class Cmp = std::less<>>
class AVLTree : public BST<T, Cmp> {
 public:

  explicit AVLTree(const Cmp &compare = Cmp());

  ~AVLTree() = default;

  void Insert(const T &key);

  bool Erase(const T &key);

  // Метод возвращает индекс, который будет соответствовать
  // индексу элемента после его добавления
  int Position(const T &key);

  //Возвращает К-ю порядковую статистику
  pair<bool, T> KStatistics(int index);

 private:

  inline int Weight(class BST<T, Cmp>::Node *node);

  inline char Height(class BST<T, Cmp>::Node *node);

  inline char BalanceFactor(class BST<T, Cmp>::Node *node);

  //Вспомогательная функция для метода Position
  int PositionTrail(class BST<T, Cmp>::Node *node, const T &key);

  //Вспомогательная ф-я для метода Kstatistics
  T StatisticsTrail(class BST<T, Cmp>::Node *node, int index);

  class BST<T, Cmp>::Node *AVLInsert(class BST<T, Cmp>::Node *root,
                                     const T &key);

  class BST<T, Cmp>::Node *AVLErase(class BST<T, Cmp>::Node *root,
                                    const T &key);

  //Функция для удаления минимума в дереве для Erase
  class BST<T, Cmp>::Node *RemoveMin(class BST<T, Cmp>::Node *root);

  class BST<T, Cmp>::Node *Balance(class BST<T, Cmp>::Node *balancing);

  void LeftRotate(class BST<T, Cmp>::Node *rotated);

  void RightRotate(class BST<T, Cmp>::Node *rotated);

};
///////////////////////////////////////////////////////////////////////////////

int main() {
  int n;
  cin >> n;
  AVLTree<int, SoldierCompare> tree;

  for (int i = 0; i < n; ++i) {
    int command, value;
    cin >> command >> value;

    if (command == 1) {       //Если добавляем солдата, то выводим его позицию и
      cout << tree.Position(value) << endl;                      //добавляем его
      tree.Insert(value);
    } else {
      tree.Erase(tree.KStatistics(value).second); //Если солдат уходит из строя, то запрашиваем к-статистику
    }                                             //у дерева и удаляем солдата на данной позиции
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
/////////_/Компаратор для солдат\_/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool SoldierCompare::operator()(const int left, const int right) const {
  return left > right;
}

///////////////////////////////////////////////////////////////////////////////
/////////_/Реализация наивного дерева\_////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<class T, class Cmp>
BST<T, Cmp>::BST(const Cmp &compare)
    :root_(nullptr), size_(0), cmp_(compare) {}

template<class T, class Cmp>
BST<T, Cmp>::~BST() {
  vector<Node *> stack;

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

template<class T, class Cmp>
void BST<T, Cmp>::Insert(const T &key) {
  ++size_;
  Node *current = root_;
  Node *previous = nullptr;

  if (root_ == nullptr) {

    root_ = new Node(key);
    return;

  }

  while (true) {
    if (current == nullptr) {
      if (cmp_(key, previous->key)) {
        previous->left = new Node(key);
      } else {
        previous->right = new Node(key);
      }
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

template<class T, class Cmp>
bool BST<T, Cmp>::Erase(const T &key) {
  pair<bool, Node*> node = Find(key);
  if (node.first) {
    DeleteNode(node.second);
    --size_;
    return true;
  } else {
    return false;
  }
}

template<class T, class Cmp>
bool BST<T, Cmp>::Search(const T &key) {
  return Find(key).first;
}

template<class T, class Cmp>
BST<T, Cmp>::Node::Node(const T &key)
    :key(key)
     , left(nullptr)
     , right(nullptr)
     , height(1)
     , weight(1) {}

template<class T, class Cmp>
class BST<T, Cmp>::Node *BST<T, Cmp>::FindMin(Node *root) {
  if (root == nullptr) {
    return nullptr;
  }
  while (root->left != nullptr) {
    root = root->left;
  }
  return root;
}

template<class T, class Cmp>
pair<bool, class BST<T, Cmp>::Node *> BST<T, Cmp>::Find(const T &key) {
  Node *current = root_;

  while (true) {
    if (current == nullptr) {
      return make_pair(false, nullptr);
    } else if (!(cmp_(current->key, key)) && !(cmp_(key, current->key))) {
      return make_pair(true, current);
    } else if (cmp_(key, current->key)) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
}

template<class T, class Cmp>
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
    while (min->left != nullptr) {
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

///////////////////////////////////////////////////////////////////////////////
/////////_/Реализация АВЛ-дерева\_/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



template<class T, class Cmp>
AVLTree<T, Cmp>::AVLTree(const Cmp &compare)
    :BST<T, Cmp>::BST(compare) {}

template<class T, class Cmp>
void AVLTree<T, Cmp>::Insert(const T &key) {
  BST<T, Cmp>::root_ = AVLInsert(BST<T, Cmp>::root_, key);
  ++BST<T, Cmp>::size_;
}

template<class T, class Cmp>
bool AVLTree<T, Cmp>::Erase(const T &key) {

  if (BST<T, Cmp>::Search(key)) {
    BST<T, Cmp>::root_ = AVLErase(BST<T, Cmp>::root_, key);
    --BST<T, Cmp>::size_;
    return true;
  } else {
    return false;
  }
}

template<class T, class Cmp>
int AVLTree<T, Cmp>::Position(const T &key) {
  return PositionTrail(BST<T, Cmp>::root_, key);
}

template<class T, class Cmp>
pair<bool, T> AVLTree<T, Cmp>::KStatistics(int index) {

  if (index >= BST<T, Cmp>::size_) {
    return make_pair(false, 0);
  }

  return make_pair(true, StatisticsTrail(BST<T, Cmp>::root_, index));
}

template<class T, class Cmp>
inline int AVLTree<T, Cmp>::Weight(class BST<T, Cmp>::Node *node) {

  if (node == nullptr) {
    return 0;
  }

  return node->weight;
}

template<class T, class Cmp>
inline char AVLTree<T, Cmp>::Height(class BST<T, Cmp>::Node *node) {

  if (node == nullptr) {
    return 0;
  }

  return node->height;

}

template<class T, class Cmp>
inline char AVLTree<T, Cmp>::BalanceFactor(class BST<T, Cmp>::Node *node) {
  return Height(node->left) - Height(node->right);
}

template<class T, class Cmp>
int AVLTree<T, Cmp>::PositionTrail(class BST<T, Cmp>::Node *node,
                                   const T &key) {

  if (node == nullptr) {                        //Дойдя до нуля, возвращает нуль
    return 0;
  }

  if (BST<T, Cmp>::cmp_(key, node->key)) {// Если ключ меньше, то рекурсивно спускается влево
    return PositionTrail(node->left, key);
  }

  return PositionTrail(node->right, key) + Weight(node->left) + 1; //Если больше, то спускается влево, добавляя
}                                                                  //вес левого поддерева и единицу

template<class T, class Cmp>
T AVLTree<T, Cmp>::StatisticsTrail(class BST<T, Cmp>::Node *node, int index) {

  if (Weight(node->left) == index) { //Если вес левой ноды равен индексу, возвращаем ее ключ
    return node->key;
  }

  if (index < Weight(node->left)) { //Индекс меньше левой ноды -> рекурсивно идем в левое поддерево
    return StatisticsTrail(node->left, index);
  }

  return StatisticsTrail(node->right, index - (Weight(node->left) + 1));
  // Индекс больше веса левой ноды -> идем в правое поддерево с индексом
  // (необходимым весом), равным инд - вес л.п. + 1
}

template<class T, class Cmp>
class BST<T, Cmp>::Node *AVLTree<T, Cmp>::AVLInsert
                                 (class BST<T, Cmp>::Node *root, const T &key) {

  if (root == nullptr) {                     //Дойдя до нуля, создаем новый лист
    root = new class BST<T, Cmp>::Node(key);
  } else {
    if (BST<T, Cmp>::cmp_(key, root->key)) {                    //Спуск по ключу
      root->left = AVLInsert(root->left, key);
    } else {
      root->right = AVLInsert(root->right, key);
    }
  }

  return Balance(root);    //Балансировка поддерева и возврат наверх по рекурсии
}

template<class T, class Cmp>
class BST<T, Cmp>::Node *AVLTree<T, Cmp>::AVLErase
                                (class BST<T, Cmp>::Node *root,  const T &key) {

  if (BST<T, Cmp>::cmp_(root->key, key)) {                      //Спуск по ключу
    root->right = AVLErase(root->right, key);
  } else if (BST<T, Cmp>::cmp_(key, root->key)) {
    root->left = AVLErase(root->left, key);
  } else {
    if (root->right == nullptr) { //Нет правого поддерева -> подвязываем левое поддерево
      class BST<T, Cmp>::Node *tmp = root->left;
      delete root;
      return tmp;
    }
    class BST<T, Cmp>::Node *min = BST<T, Cmp>::FindMin(root->right); //Есть правое поддерево -> ищем минимум
    root->right = RemoveMin(root->right);    //в правом поддереве, удаляем его и
    class BST<T, Cmp>::Node *tmp_left = root->left; //рекурсивно балансируем его
    class BST<T, Cmp>::Node *tmp_right = root->right; //Вставляем минимум вместо удаленной ноды
    delete root;
    root = min;
    min->left = tmp_left;
    min->right = tmp_right;
  }

  return Balance(root);                  //Возвращаем сбалансированное поддерево
}

template<class T, class Cmp>
class BST<T, Cmp>::Node *AVLTree<T, Cmp>::RemoveMin
                                               (class BST<T, Cmp>::Node *root) {

  if (root->left == nullptr) {           //Как только нашли минимум, подвязываем
    return root->right;                             //правого ребенка к родителю
  }

  root->left = RemoveMin(root->left);                   //Рекурсивно балансируем

  return Balance(root);
}

template<class T, class Cmp>
class BST<T, Cmp>::Node *AVLTree<T, Cmp>::Balance
                                           (class BST<T,Cmp>::Node *balancing) {

  class BST<T, Cmp>::Node *returned = balancing;

  if (BalanceFactor(balancing) == -2) { //Делаем левый поворот при необходимости
    if (BalanceFactor(balancing->right) == 1) { //Делаем большой левый поворот при необходимости
      class BST<T, Cmp>::Node *tmp = balancing->right;
      balancing->right = balancing->right->left;          //Перепривязываем ноды
      RightRotate(tmp);                            //Делаем малый правый поворот
    }
    returned = balancing->right;                             //Меняем возвращаемую ноду
    LeftRotate(balancing);                                //Делаем левый поворот

  } else if (BalanceFactor(balancing) == 2) {            //Симметричные действия
    if (BalanceFactor(balancing->left) == -1) {
      class BST<T, Cmp>::Node *tmp = balancing->left;
      balancing->left = balancing->left->right;
      LeftRotate(tmp);
    }
    returned = balancing->left;
    RightRotate(balancing);
  }

  returned->weight = Weight(returned->left) + Weight(returned->right) + 1;//Обновляем вес

  returned->height = max(Height(returned->left), Height(returned->right)) + 1;//Обновляем высоту

  return returned;
}

template<class T, class Cmp>
void AVLTree<T, Cmp>::LeftRotate(class BST<T, Cmp>::Node *rotated) {

  class BST<T, Cmp>::Node *tmp = rotated->right;
  rotated->right = tmp->left;                             //Перепривязываем ноды
  tmp->left = rotated;

  rotated->weight = Weight(rotated->left) + Weight(rotated->right) + 1; //Обновляем веса
  tmp->weight = Weight(tmp->left) + Weight(tmp->right) + 1;

  rotated->height = max(Height(rotated->left), Height(rotated->right)) + 1; //Обновляем высоты
  tmp->height = max(Height(tmp->left), Height(tmp->right)) + 1;
}

template<class T, class Cmp>
void AVLTree<T, Cmp>::RightRotate(class BST<T, Cmp>::Node *rotated) {

  class BST<T, Cmp>::Node *tmp = rotated->left;
  rotated->left = tmp->right;
  tmp->right = rotated;

  rotated->weight = Weight(rotated->left) + Weight(rotated->right) + 1;
  tmp->weight = Weight(tmp->left) + Weight(tmp->right) + 1;

  rotated->height = max(Height(rotated->left), Height(rotated->right)) + 1;
  tmp->height = max(Height(tmp->left), Height(tmp->right)) + 1;
}
