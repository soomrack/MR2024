#pragma once
#include "IHeap.h"
#include <list>
#include <vector>
#include <cmath>
#include <stdexcept>

class FibonacciHeap : public IHeap {
private:
    struct Node {
        int key;
        int degree = 0;
        bool marked = false;
        Node* parent = nullptr;
        std::list<Node*> children;

        Node(int val) : key(val) {}
    };

    std::list<Node*> root_list;
    Node* max_node = nullptr;
    size_t total_nodes = 0;

    void merge_root(Node* node) {
        root_list.push_back(node);
        if (!max_node || node->key > max_node->key)
            max_node = node;
    }

    void link(Node* y, Node* x) {
        root_list.remove(y);
        y->parent = x;
        x->children.push_back(y);
        x->degree++;
        y->marked = false;
    }

    void consolidate() {
        size_t D = static_cast<size_t>(std::log2(total_nodes)) + 2;
        std::vector<Node*> A(D, nullptr);

        std::list<Node*> old_roots = root_list;
        root_list.clear();
        max_node = nullptr;

        for (Node* w : old_roots) {
            Node* x = w;
            size_t d = x->degree;
            while (A[d]) {
                Node* y = A[d];
                if (x->key < y->key) std::swap(x, y);
                link(y, x);
                A[d] = nullptr;
                ++d;
            }
            A[d] = x;
        }

        for (Node* node : A)
            if (node) merge_root(node);
    }

public:
    void insert(int value) override {
        Node* node = new Node(value);
        merge_root(node);
        total_nodes++;
    }

    int get_max() const override {
        if (!max_node) throw std::runtime_error("Heap is empty");
        return max_node->key;
    }

    int extract_max() override {
        if (!max_node) throw std::runtime_error("Heap is empty");
        int max_value = max_node->key;

        for (Node* child : max_node->children) {
            child->parent = nullptr;
            root_list.push_back(child);
        }

        root_list.remove(max_node);
        delete max_node;
        total_nodes--;

        if (!root_list.empty()) {
            max_node = *root_list.begin();
            consolidate();
        } else {
            max_node = nullptr;
        }

        return max_value;
    }

    size_t size() const override {
        return total_nodes;
    }

    bool is_empty() const override {
        return total_nodes == 0;
    }

    void merge(IHeap& other_heap) override {
        auto* other = dynamic_cast<FibonacciHeap*>(&other_heap);
        if (!other) throw std::runtime_error("Cannot merge: incompatible heap type");

        root_list.splice(root_list.end(), other->root_list);
        if (!max_node || (other->max_node && other->max_node->key > max_node->key))
            max_node = other->max_node;
        total_nodes += other->total_nodes;

        // Clear donor heap
        other->max_node = nullptr;
        other->total_nodes = 0;
    }
};
