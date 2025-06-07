#pragma once
#include "IHeap.h"
#include <list>
#include <cmath>
#include <stdexcept>
#include <vector>

class BinomialHeap : public IHeap {
private:
    struct Node {
        int key;
        int degree = 0;
        Node* parent = nullptr;
        std::list<Node*> children;

        Node(int val) : key(val) {}
    };

    std::list<Node*> roots;
    Node* max_node = nullptr;
    size_t total_nodes = 0;

    void link(Node* a, Node* b) {
        if (a->key < b->key) std::swap(a, b);
        b->parent = a;
        a->children.push_back(b);
        a->degree++;
    }

    void consolidate() {
        std::vector<Node*> degree_map(64, nullptr);
        std::list<Node*> temp_roots = roots;
        roots.clear();
        max_node = nullptr;

        for (Node* curr : temp_roots) {
            while (degree_map[curr->degree]) {
                Node* other = degree_map[curr->degree];
                degree_map[curr->degree] = nullptr;
                link(curr, other);
            }
            degree_map[curr->degree] = curr;
        }

        for (Node* node : degree_map) {
            if (node) {
                roots.push_back(node);
                if (!max_node || node->key > max_node->key)
                    max_node = node;
            }
        }
    }

public:
    void insert(int value) override {
        BinomialHeap temp;
        temp.roots.push_back(new Node(value));
        temp.total_nodes = 1;
        merge(temp);
    }

    int get_max() const override {
        if (!max_node) throw std::runtime_error("Heap is empty");
        return max_node->key;
    }

    int extract_max() override {
        if (!max_node) throw std::runtime_error("Heap is empty");
        int max_value = max_node->key;

        roots.remove(max_node);
        for (Node* child : max_node->children) {
            child->parent = nullptr;
            roots.push_back(child);
        }
        delete max_node;
        total_nodes--;

        consolidate();
        return max_value;
    }

    size_t size() const override {
        return total_nodes;
    }

    bool is_empty() const override {
        return total_nodes == 0;
    }

    void merge(IHeap& other_heap) override {
        auto* other = dynamic_cast<BinomialHeap*>(&other_heap);
        if (!other) throw std::runtime_error("Cannot merge: incompatible heap type");

        roots.splice(roots.end(), other->roots);
        total_nodes += other->total_nodes;
        consolidate();

        other->roots.clear();
        other->total_nodes = 0;
        other->max_node = nullptr;
    }
};
