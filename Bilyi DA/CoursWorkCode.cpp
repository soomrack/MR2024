#include <iostream> 
#include <queue>
#include <stack>
#include <ctime>
#include <stdexcept>
#include <string>
#include <algorithm>

class Tree_Exception : public std::runtime_error
{
public:
    Tree_Exception(const std::string& msg) : std::runtime_error(msg)
    {
    }
    
    Tree_Exception(const char* msg) : std::runtime_error(msg)
    {
    }
};

extern Tree_Exception ALREADY_EXISTS;
extern Tree_Exception DOES_NOT_EXISTS;

typedef double T;

class NodeTree {
protected:
    T data;
    int key;
    NodeTree* left_child;
    NodeTree* right_child;
    int height;
    int bf; // balance factor = difference between heights of left and right subtrees

public:
    NodeTree(int key, T data, NodeTree* left_child = nullptr, NodeTree* right_child = nullptr);
    NodeTree(const NodeTree& node);
    ~NodeTree();
    void update(); // updates balance factor and height

    friend class Tree;
};

typedef std::stack<NodeTree**> node_stack;

class Tree {
protected:
    NodeTree* root = nullptr;

public:
    Tree() = default;
    ~Tree();

    void add(int new_key);
    void del_by_data(T del_data);
    void del_by_key(int del_key);

    T get_data(int key);
    int get_key(T data);

    void print_tree();
    void print_in_order(NodeTree* node);

    void balance(node_stack stack);
    void l_rotate(NodeTree** node);
    void r_rotate(NodeTree** node);
    void lr_rotate(NodeTree** node);
    void rl_rotate(NodeTree** node);
    
    // Вспомогательные методы для тестирования
    int get_height() { return root ? root->height : -1; }
    bool is_balanced() { return check_balance(root); }
    
private:
    bool check_balance(NodeTree* node);
};

// Определения глобальных исключений
Tree_Exception ALREADY_EXISTS("Node with this key already exists");
Tree_Exception DOES_NOT_EXISTS("Node does not exist");

NodeTree::NodeTree(int key, T data, NodeTree* left_child, NodeTree* right_child) {
    this->data = data;
    this->key = key;
    this->left_child = left_child;
    this->right_child = right_child;
    this->height = 0;
    this->bf = 0;
    update();
}

NodeTree::NodeTree(const NodeTree& node) {
    data = node.data;
    key = node.key;
    bf = node.bf;
    height = node.height;
    left_child = node.left_child;
    right_child = node.right_child;
}

NodeTree::~NodeTree() {
    delete left_child;
    delete right_child;
}

void NodeTree::update() {
    int lheight = (left_child == nullptr ? -1 : left_child->height);
    int rheight = (right_child == nullptr ? -1 : right_child->height);
    height = std::max(lheight, rheight) + 1;
    bf = lheight - rheight;
}

Tree::~Tree() {
    delete root;
}

void Tree::add(int new_key) {
    NodeTree** temp = &root;
    node_stack stack;
    
    while (*temp != nullptr) {
        if ((*temp)->key == new_key) throw ALREADY_EXISTS;
        stack.push(temp);
        
        if ((*temp)->key > new_key) {
            temp = &((*temp)->left_child);
        } else {
            temp = &((*temp)->right_child);
        }
    }
    
    T new_data = static_cast<T>(new_key);
    *temp = new NodeTree(new_key, new_data);
    balance(stack);
}

T Tree::get_data(int key) {
    NodeTree** temp = &root;
    while (*temp != nullptr) {
        if ((*temp)->key == key) {
            return (*temp)->data;
        }
        if ((*temp)->key > key) {
            temp = &((*temp)->left_child);
        } else {
            temp = &((*temp)->right_child);
        }
    }
    throw DOES_NOT_EXISTS;
}

int Tree::get_key(T data) {
    if (root == nullptr) throw DOES_NOT_EXISTS;
    
    std::queue<NodeTree*> queue;
    queue.push(root);

    while (!queue.empty()) {
        NodeTree* temp = queue.front();
        queue.pop();
        
        if (temp->data == data) {
            return temp->key;
        }
        
        if (temp->left_child != nullptr) queue.push(temp->left_child);
        if (temp->right_child != nullptr) queue.push(temp->right_child);
    }
    
    throw DOES_NOT_EXISTS;
}

void Tree::del_by_key(int del_key) {
    NodeTree** temp = &root;
    node_stack stack;
    
    while (*temp != nullptr) {
        if ((*temp)->key == del_key) {
            // Случай 1: Нет детей
            if ((*temp)->left_child == nullptr && (*temp)->right_child == nullptr) {
                delete *temp;
                *temp = nullptr;
                balance(stack);
                return;
            }
            
            // Случай 2: Один ребенок
            if ((*temp)->left_child == nullptr) {
                NodeTree* to_delete = *temp;
                *temp = (*temp)->right_child;
                to_delete->right_child = nullptr;
                delete to_delete;
                balance(stack);
                return;
            }
            
            if ((*temp)->right_child == nullptr) {
                NodeTree* to_delete = *temp;
                *temp = (*temp)->left_child;
                to_delete->left_child = nullptr;
                delete to_delete;
                balance(stack);
                return;
            }
            
            // Случай 3: Два ребенка
            NodeTree** successor = &((*temp)->right_child);
            stack.push(temp);
            
            while ((*successor)->left_child != nullptr) {
                stack.push(successor);
                successor = &((*successor)->left_child);
            }
            
            // Копируем данные
            (*temp)->key = (*successor)->key;
            (*temp)->data = (*successor)->data;
            
            // Удаляем successor
            NodeTree* to_delete = *successor;
            *successor = (*successor)->right_child;
            to_delete->right_child = nullptr;
            delete to_delete;
            
            balance(stack);
            return;
        }
        
        stack.push(temp);
        
        if ((*temp)->key > del_key) {
            temp = &((*temp)->left_child);
        } else {
            temp = &((*temp)->right_child);
        }
    }
    
    throw DOES_NOT_EXISTS;
}

void Tree::del_by_data(T del_data) {
    try {
        int key = get_key(del_data);
        del_by_key(key);
    } catch (const Tree_Exception&) {
        throw DOES_NOT_EXISTS;
    }
}

void Tree::balance(node_stack stack) {
    while (!stack.empty()) {
        NodeTree** temp = stack.top();
        stack.pop();
        
        (*temp)->update();
        
        if ((*temp)->bf < -1) {
            if ((*temp)->right_child->bf <= 0) {
                l_rotate(temp);
            } else {
                rl_rotate(temp);
            }
        } else if ((*temp)->bf > 1) {
            if ((*temp)->left_child->bf >= 0) {
                r_rotate(temp);
            } else {
                lr_rotate(temp);
            }
        }
    }
}

void Tree::l_rotate(NodeTree** node) {
    NodeTree* child = (*node)->right_child;
    (*node)->right_child = child->left_child;
    child->left_child = *node;
    *node = child;
    
    // Обновляем высоты
    (*node)->left_child->update();
    (*node)->update();
}

void Tree::r_rotate(NodeTree** node) {
    NodeTree* child = (*node)->left_child;
    (*node)->left_child = child->right_child;
    child->right_child = *node;
    *node = child;
    
    // Обновляем высоты
    (*node)->right_child->update();
    (*node)->update();
}

void Tree::lr_rotate(NodeTree** node) {
    l_rotate(&((*node)->left_child));
    r_rotate(node);
}

void Tree::rl_rotate(NodeTree** node) {
    r_rotate(&((*node)->right_child));
    l_rotate(node);
}

void Tree::print_in_order(NodeTree* node) {
    if (node == nullptr) return;
    print_in_order(node->left_child);
    std::cout << "Key: " << node->key << ", Data: " << node->data 
              << ", Height: " << node->height << ", BF: " << node->bf << std::endl;
    print_in_order(node->right_child);
}

void Tree::print_tree() {
    std::cout << "Tree (in-order):" << std::endl;
    print_in_order(root);
    std::cout << std::endl;
}

bool Tree::check_balance(NodeTree* node) {
    if (node == nullptr) return true;
    
    if (std::abs(node->bf) > 1) return false;
    
    return check_balance(node->left_child) && check_balance(node->right_child);
}

int main() {
    std::srand(std::time(0));
    
    const int iter = 100;
    Tree A;
    int values[iter];

    std::cout << "Adding " << iter << " elements..." << std::endl;
    
    for (int i = 0; i < iter; ++i) {
        int rand_val = std::rand() % 1000;
        values[i] = rand_val;

        try {
            A.add(rand_val);
        } catch (const Tree_Exception& e) {
            // Игнорируем дубликаты для теста
        }
    }

    std::cout << "Tree height: " << A.get_height() << std::endl;
    std::cout << "Tree is balanced: " << (A.is_balanced() ? "Yes" : "No") << std::endl;
    
    std::cout << "\nDeleting elements..." << std::endl;

    for (int i = 0; i < iter / 2; ++i) {
        int index = std::rand() % iter;
        int value = values[index];

        try {
            A.del_by_key(value);
        } catch (const Tree_Exception& e) {
            // Игнорируем несуществующие элементы
        }
    }

    std::cout << "Tree height after deletion: " << A.get_height() << std::endl;
    std::cout << "Tree is balanced after deletion: " << (A.is_balanced() ? "Yes" : "No") << std::endl;

    return 0;
}