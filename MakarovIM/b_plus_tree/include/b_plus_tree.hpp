#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <algorithm>

class BPlusTree {
private:
    // Node описывает узел дерева
    struct Node {
        bool is_leaf;                // признак листа
        std::vector<long> keys;     // отсортированные ключи в узле
        std::vector<Node*> children; // указатели на детей (для листьев не используется)
        Node* next;                 // указатель на следующий лист (для листьев)
        Node* parent;               // указатель на родительский узел
        Node(bool leaf = false)     
            : is_leaf(leaf), next(nullptr), parent(nullptr) {}
    };

    Node* root;                     // корень дерева

    static const int M = 4;                     // порядок дерева (макс. потомков)
    static const int MIN_CHILDREN = (M + 1) / 2;     // минимум детей 
    static const int MIN_KEYS_INTERNAL = MIN_CHILDREN - 1; // минимум ключей у внутренних узлов дерева
    static const int MIN_KEYS_LEAF = MIN_CHILDREN;        // минимум ключей в листе

    void delete_node(Node* node);     // рекурсивно удаляет поддерево (для деструктора)
    Node* find_leaf(Node* node, long key) const; // найти лист для данного ключа
    void split_leaf(Node* leaf);      // разделить переполненный лист
    void split_internal(Node* node);  // разделить переполненный внутренний узел
    void fix_internal_underflow(Node* node); // исправить недостаточную заполненность внутреннего узла
    void print_tree(Node* node, int level) const; // приватный метод для рекурсивного вывода дерева

public:
    BPlusTree();          
    ~BPlusTree();         

    bool search(long key) const; // поиск ключа
    void insert(long key);       // вставка ключа
    bool remove(long key);       // удаление ключа

    void print_tree() const;      // вывод структуры дерева
};

#endif
