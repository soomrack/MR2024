#include "b_plus_tree.hpp"
#include <iostream>

// Вспомогательный рекурсивный метод для удаления всех узлов (вызывается из деструктора)
void BPlusTree::delete_node(Node* node) {
    if (!node) return;
    if (!node->is_leaf) {
        // рекурсивно удаляем всех детей
        for (Node* c : node->children) {
            delete_node(c);
        }
    }
    delete node;
}


BPlusTree::BPlusTree() : root(nullptr) {}
BPlusTree::~BPlusTree() {
    delete_node(root);
}


// Поиск листа, куда должен попасть ключ (или где он хранится)
    BPlusTree::Node* BPlusTree::find_leaf(Node* node, long key) const {
        if (!node) return nullptr;
        if (node->is_leaf) return node;
        // найти первый ключ > key
        int i = 0;
        while (i < node->keys.size() && key >= node->keys[i]) {
            i++;
        }
        // переходим к соответствующему потомку
        return find_leaf(node->children[i], key);
    }


bool BPlusTree::search(long key) const {
    Node* leaf = find_leaf(root, key);
    if (!leaf) return false;
    // в листе обычным поиском смотрим, есть ли ключ
    return std::find(leaf->keys.begin(), leaf->keys.end(), key) != leaf->keys.end();
}


void BPlusTree::insert(long key) {
    if (!root) {
        // пустое дерево: создаём лист-корень
        root = new Node(true);
        root->keys.push_back(key);
        return;
    }
    // находим лист для вставки
    Node* leaf = find_leaf(root, key);
    // вставляем ключ в отсортированном порядке
    auto it = std::upper_bound(leaf->keys.begin(), leaf->keys.end(), key);
    leaf->keys.insert(it, key);
    // если лист переполнен (больше M ключей), разделяем его
    if (leaf->keys.size() > M) {
        split_leaf(leaf);
    }
}


// Разделение листа при переполнении
void BPlusTree::split_leaf(Node* leaf) {
    // создаём новый лист
    Node* new_leaf = new Node(true);
    new_leaf->parent = leaf->parent;
    // переносим половину ключей в новый лист
    int total = leaf->keys.size();
    int mid = (total + 1) / 2;
    new_leaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
    leaf->keys.resize(mid);
    // исправляем связи листов
    new_leaf->next = leaf->next;
    leaf->next = new_leaf;
    // ключ, продвигаемый вверх – первый ключ нового листа
    long new_key = new_leaf->keys.front();

    if (!leaf->parent) {
        // если не было родителя, создаём новый корень
        Node* new_root = new Node(false);
        new_root->keys.push_back(new_key);
        new_root->children.push_back(leaf);
        new_root->children.push_back(new_leaf);
        leaf->parent = new_root;
        new_leaf->parent = new_root;
        root = new_root;
    } else {
        // вставляем новый ключ и указатель в родителя
        Node* parent = leaf->parent;
        // найти позицию листа среди детей
        int idx = 0;
        while (parent->children[idx] != leaf) idx++;
        parent->keys.insert(parent->keys.begin() + idx, new_key);
        parent->children.insert(parent->children.begin() + idx + 1, new_leaf);
        new_leaf->parent = parent;
        // если родитель переполнен по числу детей, разделяем его
        if ((int)parent->children.size() > M) {
            split_internal(parent);
        }
    }
}


// Разделение внутреннего узла при переполнении
void BPlusTree::split_internal(Node* node) {
    Node* parent = node->parent;
    int total_keys = node->keys.size();
    int mid_index = total_keys / 2;
    long mid_key = node->keys[mid_index]; // ключ, который уйдёт вверх

    // создаём новый узел
    Node* newNode = new Node(false);
    newNode->parent = parent;
    // переносим ключи после mid_index в новый узел
    for (int i = mid_index + 1; i < total_keys; ++i) {
        newNode->keys.push_back(node->keys[i]);
    }
    // переносим соответствующие дочерние указатели
    int totalChildren = node->children.size();
    for (int i = mid_index + 1; i < totalChildren; ++i) {
        newNode->children.push_back(node->children[i]);
        newNode->children.back()->parent = newNode;
    }
    // обрезаем старый узел
    node->keys.resize(mid_index);
    node->children.resize(mid_index + 1);

    if (!parent) {
        // если делился корень, создаём новый корень
        Node* new_root = new Node(false);
        new_root->keys.push_back(mid_key);
        new_root->children.push_back(node);
        new_root->children.push_back(newNode);
        node->parent = new_root;
        newNode->parent = new_root;
        root = new_root;
    } else {
        // вставляем mid_key в родителя и указатель на newNode
        int idx = 0;
        while (idx < (int)parent->keys.size() && parent->keys[idx] < mid_key) idx++;
        parent->keys.insert(parent->keys.begin() + idx, mid_key);
        parent->children.insert(parent->children.begin() + idx + 1, newNode);
        newNode->parent = parent;
        if ((int)parent->children.size() > M) {
            split_internal(parent);
        }
    }
}

// Исправление недостаточной заполненности внутреннего узла после удаления
void BPlusTree::fix_internal_underflow(Node* node) {
    Node* parent = node->parent;
    if (!parent) return;
    // найти индекс узла среди детей родителя
    int idx = 0;
    while (parent->children[idx] != node) idx++;
    Node* left = (idx > 0) ? parent->children[idx - 1] : nullptr;
    Node* right = (idx + 1 < (int)parent->children.size()) ? parent->children[idx + 1] : nullptr;

    //  заимствования из левого соседа
    if (left && (int)left->children.size() > MIN_CHILDREN) {
        long borrow_key = parent->keys[idx - 1];
        Node* child = left->children.back();
        left->children.pop_back();
        long movedKey = left->keys.back();
        left->keys.pop_back();
        // помещаем borrow_Key в начале текущего узла
        node->children.insert(node->children.begin(), child);
        node->keys.insert(node->keys.begin(), borrow_key);
        child->parent = node;
        parent->keys[idx - 1] = movedKey;
        return;
    }
    // Попытка заимствования из правого соседа
    if (right && (int)right->children.size() > MIN_CHILDREN) {
        long borrow_key = parent->keys[idx];
        Node* child = right->children.front();
        right->children.erase(right->children.begin());
        long movedKey = right->keys.front();
        right->keys.erase(right->keys.begin());
        node->children.push_back(child);
        node->keys.push_back(borrow_key);
        child->parent = node;
        parent->keys[idx] = movedKey;
        return;
    }
    // Слияние узлов: если можно, сливаем с левым, иначе с правым
    if (left) {
        long sep_key = parent->keys[idx - 1];
        left->keys.push_back(sep_key);
        for (long k : node->keys) left->keys.push_back(k);
        for (Node* c : node->children) {
            left->children.push_back(c);
            c->parent = left;
        }
        parent->children.erase(parent->children.begin() + idx);
        parent->keys.erase(parent->keys.begin() + idx - 1);
        delete node;
    } else if (right) {
        long sep_key = parent->keys[idx];
        node->keys.push_back(sep_key);
        for (long k : right->keys) node->keys.push_back(k);
        for (Node* c : right->children) {
            node->children.push_back(c);
            c->parent = node;
        }
        parent->children.erase(parent->children.begin() + idx + 1);
        parent->keys.erase(parent->keys.begin() + idx);
        delete right;
    }
    // Если родитель – корень и в нём остался один ребёнок, поднимаем его как новый корень
    if (parent == root && parent->keys.empty()) {
        Node* only = parent->children[0];
        only->parent = nullptr;
        root = only;
        delete parent;
    } else if (parent->children.size() < MIN_CHILDREN) {
        // рекурсивно исправляем родителя
        fix_internal_underflow(parent);
    }
}

bool BPlusTree::remove(long key) {
    Node* leaf = find_leaf(root, key);
    if (!leaf) return false;
    auto it = std::find(leaf->keys.begin(), leaf->keys.end(), key);
    if (it == leaf->keys.end()) return false; // ключ не найден
    // удаляем ключ из листа
    leaf->keys.erase(it);
    // если это был единственный ключ в дереве, очищаем дерево
    if (leaf == root) {
        if (leaf->keys.empty()) {
            delete root;
            root = nullptr;
        }
        return true;
    }
    // если узел после удаления ещё заполнен минимум на ⌈M/2⌉, балансировка не нужна
    if ((int)leaf->keys.size() >= MIN_KEYS_LEAF) return true;

    // иначе выполняем процедуру заимствования/слияния аналогично внутренним узлам
    Node* parent = leaf->parent;
    int idx = 0;
    while (parent->children[idx] != leaf) idx++;
    Node* left = (idx > 0) ? parent->children[idx - 1] : nullptr;
    Node* right = (idx + 1 < (int)parent->children.size()) ? parent->children[idx + 1] : nullptr;

    //  заимствования из левого листа
    if (left && (int)left->keys.size() > MIN_KEYS_LEAF) {
        long borrowed = left->keys.back();
        left->keys.pop_back();
        leaf->keys.insert(leaf->keys.begin(), borrowed);
        // обновляем соответствующий ключ в родителе
        parent->keys[idx - 1] = leaf->keys.front();
        return true;
    }
    // Заимствование из правого листа
    if (right && (int)right->keys.size() > MIN_KEYS_LEAF) {
        long borrowed = right->keys.front();
        right->keys.erase(right->keys.begin());
        leaf->keys.push_back(borrowed);
        parent->keys[idx] = right->keys.front();
        return true;
    }
    // Слияние листов
    if (left) {
        // сливаем текущий лист в левый
        for (long k : leaf->keys) left->keys.push_back(k);
        left->next = leaf->next;
        parent->children.erase(parent->children.begin() + idx);
        parent->keys.erase(parent->keys.begin() + idx - 1);
        delete leaf;
    } else if (right) {
        // сливаем правый лист в текущий
        for (long k : right->keys) leaf->keys.push_back(k);
        leaf->next = right->next;
        parent->children.erase(parent->children.begin() + idx + 1);
        parent->keys.erase(parent->keys.begin() + idx);
        delete right;
    }
    // Если после слияния родитель стал почти пустым, исправляем его
    if (parent == root && parent->children.size() == 1) {
        Node* only = parent->children[0];
        only->parent = nullptr;
        root = only;
        delete parent;
        return true;
    }
    if (parent->children.size() < MIN_CHILDREN) {
        fix_internal_underflow(parent);
    }
    return true;
}

// Вывод дерева
void BPlusTree::print_tree(Node* node, int level) const {
    if (!node) return;

    for (int i = 0; i < level; ++i)
        std::cout << "    ";

    if (node->is_leaf) {
        std::cout << "Leaf: ";
        for (auto k : node->keys)
            std::cout << k << " ";
        std::cout << "\n";
    } else {
        std::cout << "Node: ";
        for (auto k : node->keys)
            std::cout << k << " ";
        std::cout << "\n";
        for (auto c : node->children)
            print_tree(c, level + 1);
    }
}

void BPlusTree::print_tree() const {
    print_tree(root, 0);
}
