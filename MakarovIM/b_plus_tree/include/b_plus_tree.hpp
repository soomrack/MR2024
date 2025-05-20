#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <algorithm>
#include "b_plus_tree_node.hpp"

class BPlusTree {
private:
    BPlusTreeNode* root;                     // корень дерева

    static const int M = 4;                     // порядок дерева (макс. потомков)
    static const int MIN_CHILDREN = (M + 1) / 2;     // минимум детей 
    static const int MIN_KEYS_INTERNAL = MIN_CHILDREN - 1; // минимум ключей у внутренних узлов дерева
    static const int MIN_KEYS_LEAF = MIN_CHILDREN;        // минимум ключей в листе

    void delete_node(BPlusTreeNode* node);     // рекурсивно удаляет поддерево (для деструктора)
    BPlusTreeNode* find_leaf(BPlusTreeNode* node, long key) const; // найти лист для данного ключа
    void split_leaf(BPlusTreeNode* leaf);      // разделить переполненный лист
    void split_internal(BPlusTreeNode* node);  // разделить переполненный внутренний узел
    void fix_internal_underflow(BPlusTreeNode* node); // исправить недостаточную заполненность внутреннего узла
    void print_tree(BPlusTreeNode* node, int level) const; // приватный метод для рекурсивного вывода дерева

public:
    BPlusTree();          
    ~BPlusTree();         

    bool search(long key) const; // поиск ключа
    void insert(long key);       // вставка ключа
    bool remove(long key);       // удаление ключа

    void print_tree() const;      // вывод структуры дерева
};

#endif
