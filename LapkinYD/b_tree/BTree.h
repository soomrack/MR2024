#ifndef B_TREE_H
#define B_TREE_H

#include <iostream>
#include <ctime>
#include <random>

const int T = 3;  // Минимальная степень дерева

struct Node {
    Node* parent;
    Node* children[2 * T];
    double keys[2 * T - 1];
    int keysCount;
};

class Tree {
public:
    Node* root;
    
    void insert(double value);
    double* search(double value);
    void remove(double value);
    
    Tree() {
        root = new Node;
        root->keysCount = 0;
        root->parent = nullptr;
        
        for (int i = 0; i < 2 * T; i++) {
            root->children[i] = nullptr;
        }
    }

private:
    void insertToNode(Node* currentNode, double value);
    Node* createNode(Node* parent);
    Node* searchNode(Node* currentNode, double value);
    double* searchInNode(Node* currentNode, double value);
    void removeFromNode(Node* currentNode, double value);
    void sortChildPointers(Node* currentNode);
    Node* createRootParent();
    void sortKeys(Node* currentNode);
    void removeFromLeaf(Node* currentNode, double value);
    void mergeNodes(Node* firstNode, Node* secondNode);
};

#endif // B_TREE_H