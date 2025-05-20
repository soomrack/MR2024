#ifndef BPLUSTREE_NODE_H
#define BPLUSTREE_NODE_H

#include <vector>

class BPlusTreeNode {
public:
    std::vector<long> keys;     // отсортированные ключи в узле
    std::vector<BPlusTreeNode*> children; // указатели на детей
    BPlusTreeNode* next;        // указатель на следующий лист (для листьев)

    BPlusTreeNode() : next(nullptr) {}
    ~BPlusTreeNode() = default;

    // Проверка является ли узел листом
    bool is_leaf() const {
        return children.empty();
    }

    // Получение родителя через поиск в дереве
    BPlusTreeNode* get_parent(const BPlusTreeNode* root) const {
        if (this == root) return nullptr;
        
        // Рекурсивный поиск родителя
        for (auto* child : root->children) {
            if (child == this) return const_cast<BPlusTreeNode*>(root);
            auto* parent = get_parent(child);
            if (parent) return parent;
        }
        return nullptr;
    }
};

#endif 