#include <iostream>
#include <string.h>

class Node
{
public:
    int key;
    bool color;
    std::string name;

    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;

    Node(int key, std::string name);

    void display();
    std::string search_name(int f_key);
    Node* search_node(int f_key);
    Node* get_min();
    Node* get_max();
    void delete_node();
};


Node::Node(int key, std::string name): key(key), name(name), left(nullptr), right(nullptr), parent(nullptr){}


void Node::display(){
    if(left != nullptr) left->display();
    std::cout << "Name " << name << " Key " << key;
    if(parent != nullptr){
        std::cout << " Parent " << parent->key;
    }
    else std::cout << " Parent - root";
    std::cout << " " << std::endl;
    if(right != nullptr) right->display();
}


std::string Node::search_name(int f_key){
    if(key == f_key)return name;
    if(f_key < key && left != nullptr)return left->search_name(f_key);
    if(f_key > key && right != nullptr)return right->search_name(f_key);
    return "not found";
}


Node* Node::search_node(int f_key){
    if(key == f_key)return this;
    if(f_key < key && left != nullptr)return left->search_node(f_key);
    if(f_key > key && right != nullptr)return right->search_node(f_key);
    return nullptr;
}


Node* Node::get_min(){
    if(this == nullptr)return nullptr;
    if(left == nullptr)return this;
    return left->get_min();
}


Node* Node::get_max(){
    if(this == nullptr)return nullptr;
    if(right == nullptr)return this;
    return right->get_max();
}


void Node::delete_node(){
    if(this == nullptr)return;
    if(left == nullptr && right == nullptr){
        if(this == parent->left) {
            parent->left = nullptr;
        }
        else parent->right = nullptr;
        return;
    }
    if(left == nullptr && right != nullptr){
        if(this == parent->left)parent->left = right;
        else parent->right = right;
        right->parent = parent;
        return;
    }
    if(left != nullptr && right == nullptr){
        if(this == parent->left) parent->left = left;
        else parent->right = left;
        left->parent = parent;
        return;
    }
    Node* max_node = this->get_max();
    this->key = max_node->key;
    this->name = max_node->name;
    max_node->delete_node();
}


class Tree 
{
private:
    Node *root;
public:
    Tree() : root(nullptr) {}

// Destructor
    ~Tree() {
        //clear(root);
    }

    void add_node(int key, const std::string& name);
    void display();

    std::string search_name(int key);
    Node* search_node(int key);

    Node* get_min();
    Node* get_max();
    void delete_node(int d_key);
};


void Tree::add_node(int key, const std::string& name) {
    Node *newNode = new Node(key, name);

    if (root == nullptr) {
        root = newNode;
        root->color = 0;
        return;
    }

    Node* current = root;
    Node* parent = nullptr;

    while(current != nullptr){
        parent = current;
        if(key < current->key){
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    newNode->parent = parent;
    if(key < parent->key){
        parent->left = newNode;
    }
    else {
        parent->right = newNode;
    }
}


void Tree::display(){
    root->display();
}


std::string Tree::search_name(int key){
    return root->search_name(key);
}


Node* Tree::search_node(int key){
    return root->search_node(key);
}


Node* Tree::get_min(){
    return root->get_min();
}


Node* Tree::get_max(){
    return root->get_max();
}


void Tree::delete_node(int d_key){
    search_node(d_key)->delete_node();
}


int main()
{
    Tree tree1;
    tree1.add_node(10, "Moscow");
    tree1.add_node(3, "London");
    tree1.add_node(11, "Paris");
    tree1.add_node(1, "New York");
    tree1.add_node(4, "Madrid");
    tree1.display();
    std::cout<< tree1.search_name(11) << std::endl;
    std::cout<< tree1.search_node(11)->name << std::endl;
    std::cout<< tree1.get_min()->key << std::endl;
    std::cout<< tree1.get_max()->key << std::endl;
    tree1.delete_node(3);
    tree1.display();
}