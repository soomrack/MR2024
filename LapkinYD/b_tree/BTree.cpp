#include "BTree.h"
#include <iostream>

Node* Tree::createNode(Node* parent) {
    Node* newNode = new Node;
    newNode->keysCount = 0;
    newNode->parent = parent;
    
    for (int i = 0; i < 2 * T; i++) {
        newNode->children[i] = nullptr;
    }
    
    for (int i = 0; i < 2 * T - 1; i++) {
        newNode->keys[i] = 0;
    }
    
    return newNode;
}

Node* Tree::createRootParent() {
    Node* newParent = new Node;
    root->parent = newParent;
    newParent->children[0] = root;
    newParent->keysCount = 0;
    root = newParent;
    newParent->parent = nullptr;
    
    for (int i = 1; i < 2 * T; i++) {
        newParent->children[i] = nullptr;
    }
    
    for (int i = 1; i < 2 * T - 1; i++) {
        newParent->keys[i] = 0;
    }
    
    return newParent;
}

void Tree::sortChildPointers(Node* currentNode) {
    int i = 1;
    int j = 2;
    Node* temp;
    
    while (i < currentNode->keysCount + 1) {
        if ((currentNode->children[i] == nullptr) || 
            (currentNode->children[i]->keys[0] > currentNode->children[i-1]->keys[0])) {
            i = j;
            j++;
        } else {
            temp = currentNode->children[i];
            currentNode->children[i] = currentNode->children[i-1];
            currentNode->children[i-1] = temp;
            i--;
            
            if (i == 0) {
                i = j;
                j++;
            }
        }
    }
}

void Tree::sortKeys(Node* currentNode) {
    int i = 1;
    int j = 2;
    double tempKey;
    Node* tempChild;
    
    while (i < currentNode->keysCount) {
        if (currentNode->keys[i] > currentNode->keys[i-1]) {
            i = j;
            j++;
        } else {
            tempKey = currentNode->keys[i];
            tempChild = currentNode->children[i];

            currentNode->keys[i] = currentNode->keys[i-1];
            currentNode->children[i] = currentNode->children[i-1];

            currentNode->keys[i-1] = tempKey;
            currentNode->children[i-1] = tempChild;

            i--;
            
            if (i == 0) {
                i = j;
                j++;
            }
        }
    }
}

void Tree::insert(double value) {
    using namespace std;
    Node* targetNode = searchNode(root, value);
    double* position = searchInNode(targetNode, value);
    
    if (position != nullptr) {
        cout << value << " already exists at address " << search(value) << endl;
        return;
    }
    insertToNode(targetNode, value);
}

void Tree::insertToNode(Node* currentNode, double value) {
    if (currentNode->keysCount < 2 * T - 1) {
        currentNode->keys[currentNode->keysCount] = value;
        currentNode->keysCount++;
        sortKeys(currentNode);
    } else {
        if (currentNode->parent == nullptr) {
            createRootParent();
        }
        insertToNode(currentNode->parent, currentNode->keys[T-1]);
        
        double middleKey = currentNode->keys[T-1];
        Node* parent = currentNode->parent;
        parent->children[parent->keysCount] = createNode(parent);
        Node* rightChild = parent->children[parent->keysCount];
        
        int index = 0;
        for (int i = T; i < 2 * T - 1; i++) {
            rightChild->keys[index] = currentNode->keys[i];
            rightChild->keysCount++;
            currentNode->keys[i] = 0;
            currentNode->keysCount--;
            
            if (currentNode->children[i] != nullptr) {
                rightChild->children[index] = currentNode->children[i];
                currentNode->children[i]->parent = rightChild;
                currentNode->children[i] = nullptr;
            }
            index++;
        }
        
        if (currentNode->children[2 * T - 1] != nullptr) {
            rightChild->children[index] = currentNode->children[2 * T - 1];
            currentNode->children[2 * T - 1]->parent = rightChild;
            currentNode->children[2 * T - 1] = nullptr;
        }

        currentNode->keys[T-1] = 0;
        currentNode->keysCount--;
        
        if (value < middleKey) {
            currentNode->keys[currentNode->keysCount] = value;
            currentNode->keysCount++;
            sortKeys(currentNode);
        } else {
            rightChild->keys[rightChild->keysCount] = value;
            rightChild->keysCount++;
            sortKeys(rightChild);
        }
        
        sortKeys(parent);
        sortChildPointers(parent);
    }
}

double* Tree::search(double value) {
    Node* targetNode = searchNode(root, value);
    return searchInNode(targetNode, value);
}

Node* Tree::searchNode(Node* currentNode, double value) {
    if (currentNode->keysCount == 0) return currentNode;
    
    if (value > currentNode->keys[currentNode->keysCount - 1]) {
        if (currentNode->children[0] == nullptr) {
            return currentNode;
        }
        return searchNode(currentNode->children[currentNode->keysCount], value);
    }
    
    for (int i = 0; i < currentNode->keysCount; i++) {
        if (value == currentNode->keys[i]) {
            return currentNode;
        }
        if (value < currentNode->keys[i]) {
            if (currentNode->children[i] == nullptr) {
                return currentNode;
            }
            return searchNode(currentNode->children[i], value);
        }
    }
    
    return nullptr;
}

double* Tree::searchInNode(Node* currentNode, double value) {
    for (int i = 0; i < currentNode->keysCount; i++) {
        if (value == currentNode->keys[i]) {
            return &currentNode->keys[i];
        }
    }
    return nullptr;
}

void Tree::remove(double value) {
    removeFromNode(searchNode(root, value), value);
    
    if (root->keysCount == 0 && root->children[0] != nullptr) {
        Node* newRoot = root->children[0];
        delete root;
        root = newRoot;
        root->parent = nullptr;
    }
}

void Tree::removeFromNode(Node* currentNode, double value) {
    if (currentNode->children[0] == nullptr) {
        removeFromLeaf(currentNode, value);
    } else {
        Node* targetNode = currentNode;
        int keyIndex;
        
        for (keyIndex = 0; keyIndex < currentNode->keysCount; keyIndex++) {
            if (value == currentNode->keys[keyIndex]) break;
        }
        
        currentNode = currentNode->children[keyIndex];
        if (currentNode->children[0] != nullptr) {
            do {
                currentNode = currentNode->children[currentNode->keysCount];
            } while (currentNode->children[0] != nullptr);
        }
        
        double replacementKey = currentNode->keys[currentNode->keysCount - 1];
        removeFromNode(currentNode, replacementKey);
        targetNode->keys[keyIndex] = replacementKey;
    }
}

void Tree::removeFromLeaf(Node* currentNode, double value) {
    for (int i = 0; i < currentNode->keysCount; i++) {
        if (value == currentNode->keys[i]) {
            for (int k = i; k < currentNode->keysCount; k++) {
                currentNode->keys[k] = currentNode->keys[k + 1];
            }
            currentNode->keysCount--;
            break;
        }
    }
    
    if (currentNode->parent == nullptr || currentNode->keysCount >= T - 1) {
        return;
    }
    
    int childIndex = -1;
    for (int i = 0; i <= currentNode->parent->keysCount; i++) {
        if (currentNode->parent->children[i] == currentNode) {
            childIndex = i;
            break;
        }
    }
    
    Node* leftSibling = (childIndex > 0) ? currentNode->parent->children[childIndex - 1] : nullptr;
    Node* rightSibling = (childIndex < currentNode->parent->keysCount) ? 
                         currentNode->parent->children[childIndex + 1] : nullptr;
    
    if (rightSibling != nullptr && rightSibling->keysCount > T - 1) {
        // Заимствование у правого соседа
        insertToNode(currentNode, currentNode->parent->keys[childIndex]);
        
        if (currentNode->children[0] != nullptr) {
            currentNode->children[currentNode->keysCount] = rightSibling->children[0];
            rightSibling->children[0]->parent = currentNode;
        }
        
        currentNode->parent->keys[childIndex] = rightSibling->keys[0];
        removeFromLeaf(rightSibling, rightSibling->keys[0]);
        return;
    }
    
    if (leftSibling != nullptr && leftSibling->keysCount > T - 1) {
        // Заимствование у левого соседа
        insertToNode(currentNode, currentNode->parent->keys[childIndex - 1]);
        
        if (currentNode->children[0] != nullptr) {
            for (int i = currentNode->keysCount; i > 0; i--) {
                currentNode->children[i] = currentNode->children[i - 1];
            }
            currentNode->children[0] = leftSibling->children[leftSibling->keysCount];
            leftSibling->children[leftSibling->keysCount]->parent = currentNode;
        }
        
        currentNode->parent->keys[childIndex - 1] = leftSibling->keys[leftSibling->keysCount - 1];
        removeFromLeaf(leftSibling, leftSibling->keys[leftSibling->keysCount - 1]);
        return;
    }
    
    if (rightSibling != nullptr && rightSibling->keysCount <= T - 1) {
        mergeNodes(currentNode, rightSibling);
        return;
    }
    
    if (leftSibling != nullptr && leftSibling->keysCount <= T - 1) {
        mergeNodes(leftSibling, currentNode);
        return;
    }
}

void Tree::mergeNodes(Node* leftNode, Node* rightNode) {
    int childIndex;
    for (childIndex = 0; childIndex <= leftNode->parent->keysCount; childIndex++) {
        if (leftNode->parent->children[childIndex] == leftNode) {
            break;
        }
    }
    
    insertToNode(leftNode, leftNode->parent->keys[childIndex]);
    
    int index = leftNode->keysCount;
    for (int i = 0; i < rightNode->keysCount; i++) {
        leftNode->keys[index] = rightNode->keys[i];
        leftNode->children[index] = rightNode->children[i];
        if (rightNode->children[i] != nullptr) {
            rightNode->children[i]->parent = leftNode;
        }
        index++;
    }
    leftNode->keysCount += rightNode->keysCount;
    
    // Последний ребенок правого узла
    leftNode->children[index] = rightNode->children[rightNode->keysCount];
    if (rightNode->children[rightNode->keysCount] != nullptr) {
        rightNode->children[rightNode->keysCount]->parent = leftNode;
    }
    
    // Удаляем правый узел из родителя
    delete leftNode->parent->children[childIndex + 1];
    leftNode->parent->children[childIndex + 1] = nullptr;
    
    // Сдвигаем оставшихся детей родителя
    for (int i = childIndex + 1; i < leftNode->parent->keysCount; i++) {
        leftNode->parent->children[i] = leftNode->parent->children[i + 1];
    }
    
    removeFromLeaf(leftNode->parent, leftNode->parent->keys[childIndex]);
}