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

    Node();
    Node(int key, std::string name);
};


Node::Node(): key(), name("nill"), left(nullptr), right(nullptr), color(0){}

Node *nill = new Node();

Node::Node(int key, std::string name): key(key), name(name), left(nill), right(nill), color(1){}


class Tree 
{
private:
    void display_subtree(Node* node);

    Node* get_sub_min(Node *node);
    Node* get_sub_min_parent(Node *node);
    Node* get_sub_max(Node *node);

    int black_child_count(Node *node);
    int child_count(Node *node);

    void swap_node(Node *node1, Node *node2);

    void right_rotate(Node *node);
    void left_rotate(Node *node);
public:
    Node *root;

    Tree() : root(nullptr) {}

    void add_node(int key, const std::string& name);
    void delete_node(int d_key);
    void display();

    Node* search_node(int key);
    Node* get_min();
    Node* get_max();
};


void Tree::add_node(int key, const std::string& name) {
    Node *newNode = new Node(key, name);

    //создание корня
    if (root == nullptr) {
        root = newNode;
        root->color = 0;
        return;
    }

    Node* current = root;
    Node* parent = nullptr;
    Node* grandpa = nullptr;
    Node* uncle = nullptr;

    //поиск места для вставки узла
    while(current != nill){
        grandpa = parent;
        parent = current;
        if(grandpa == nullptr)uncle = nullptr;
        else uncle = grandpa->right == parent ? grandpa->left : grandpa->right;
        if(key < current->key){
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    //вставка
    if(key < parent->key){
        parent->left = newNode;
    }
    else {
        parent->right = newNode;
    }

    //балансировка
    while(parent->color == 1){
        if(parent == grandpa->left){
            if(uncle != nullptr && uncle->color == 1){
                parent->color = 0;
                uncle->color = 0;
                grandpa->color = 1;
            }
            else {
                if(newNode == parent->right){
                    this->left_rotate(parent);
                }
                parent->color = 0;
                grandpa->color = 1;
                this->right_rotate(grandpa);
                Node* buffer = parent;
                parent = grandpa;
                grandpa = buffer;
            }
        }
        else{
            if(uncle != nullptr && uncle->color == 1){
                parent->color = 0;
                uncle->color = 0;
                grandpa->color = 1;
            }
            else {
                if(newNode == parent->left){
                    this->right_rotate(parent);
                }
                parent->color = 0;
                grandpa->color = 1;
                this->left_rotate(grandpa);
                Node* buffer = parent;
                parent = grandpa;
                grandpa = buffer;
            }
        }
        this->root->color = 0;
    }
}


void Tree::display_subtree(Node* node){
    if(node->left != nill) this->display_subtree(node->left);
    std::cout << "Name " << node->name << " Key " << node->key << " color " << node->color;
    if(node->left != nill)std::cout << " left child " << node->left->key;
    if(node->right != nill)std::cout << " right child "<< node->right->key;
    std::cout << std::endl;
    if(node->right != nill) this->display_subtree(node->right);
}


void Tree::display(){
    this->display_subtree(root);
}


Node* Tree::search_node(int f_key){
    Node *node = root;
    while(node != nill){
        if(node->key == f_key)return node;
        f_key < node->key ? node = node->left : node = node->right;
    }
    return nullptr;
}


Node* Tree::get_sub_min(Node* node){
    if(node->left == nill)return node;
    return this->get_sub_min(node->left);
}


Node* Tree::get_sub_min_parent(Node* node){
    if(node->left->left == nill)return node;
    return this->get_sub_min_parent(node->left);
}



Node* Tree::get_sub_max(Node* node){
    if(node->right == nill)return node;
    return this->get_sub_min(node->right);
}


Node* Tree::get_min(){
    return this->get_sub_min(root);
}


Node* Tree::get_max(){
    return this->get_sub_max(root);
}


int Tree::black_child_count(Node *node){
    int count = 0;
    if(node->left == nill)count ++;
    else if(node->left->color == 0)count ++;
    if(node->right == nill)count ++;
    else if(node->right->color == 0)count ++;
    return count;
}


int Tree::child_count(Node *node){
    int count = 0;
    if(node->left != nill)count ++;
    if(node->right != nill)count ++;
    return count;
}


void Tree::delete_node(int d_key){
    Node* current = root;
    Node* parent = nullptr;
    Node* grandpa = nullptr;
    Node* uncle = nullptr;
    Node* brother = nullptr;

    //поиск удаляемого элемента и запись его родственников
    while(current->key != d_key){
        grandpa = parent;
        parent = current;

        if(grandpa == nullptr)uncle = nullptr;
        else uncle = grandpa->right == parent ? grandpa->left : grandpa->right;
        if(parent == nullptr)brother = nullptr;
        else brother = parent->right == current ? parent->left : parent->right;
        if(d_key < current->key){
            current = current->left;
        }
        else {
            current = current->right;
        }
        if(current == nill)return;
    }


    // само удаление
    //удаление красного узла без детей
    if(this->child_count(current) == 0 && current->color == 1){
        if(current == parent->left)parent->left = nill;
        else parent->right = nill;
        return;
    }
    //удаление черного узла с 1 красным ребенком
    if(this->child_count(current) == 1 && current->color == 0 && (current->right->color == 1 || current->left->color == 1)){
        if(current->right->color == 1){
            current->key = current->right->key;
            current->name = current->right->name;
            current->right = nill;
        }
        else {
            current->key = current->left->key;
            current->name = current->left->name;
            current->left = nill;
        }
        return;
    }
    //удаление черного узла и балансировка
    if(this->child_count(current) == 0 && current->color == 0){
        if(brother->color == 1){
            parent->color = 1;
            brother->color = 1;
            if(current == parent->left)this->left_rotate(parent);
            else this->right_rotate(parent);
        }
        else{
            if(this->black_child_count(brother) == 2){
                if(parent == root){
                    brother->color = 1;
                }
                else{
                    if(parent->color == 1){
                        brother->color = 1;
                        parent->color == 0;
                    }
                    else{
                        parent->color == 1;
                        if(current == parent->left)this->left_rotate(parent);
                        else this->right_rotate(parent);
                    }
                }
            }
            else{
                if(current == parent->left){
                    if(brother->right->color == 0){
                        brother->color = 1;
                        brother->left->color = 0;
                        this->right_rotate(brother);
                        brother = parent->right;
                    }
                    brother->color = parent->color;
                    brother->right->color = 0;
                    parent->color = 0;
                    this->left_rotate(parent);
                }
                else{
                    if(brother->left->color == 0){
                        brother->color = 1;
                        brother->right->color = 0;
                        this->left_rotate(brother);
                        brother = parent->left;
                    }
                    brother->color = parent->color;
                    brother->left->color = 0;
                    parent->color = 0;
                    this->right_rotate(parent);
                }
            }
        }
        return;
    }
    //замена узла с 2 детьми на минимальный правый и рекурсивное удаление минимального правого
    if(this->child_count(current) == 2){
        Node *last = current;
        Node *min_node = get_sub_min(current->right);
        int replace_key = min_node->key;
        std::string replace_name = min_node->name;
        this->delete_node(min_node->key);
        current->key = replace_key;
        current->name = replace_name;
    }
}


void Tree::swap_node(Node *node1, Node *node2){
    int key1 = node1->key;
    std::string name1 = node1->name;
    bool color1 = node1->color;
    node1->key = node2->key;
    node1->name = node2->name;
    node1->color = node2->color;
    node2->key = key1;
    node2->name = name1;
    node2->color = color1;
}


void Tree::right_rotate(Node *node){
    this->swap_node(node, node->left);
    Node *buffer = node->right;
    node->right = node->left;
    node->left = node->right->left;
    node->right->left = node->right->right;
    node->right->right = buffer;
}


void Tree::left_rotate(Node *node){
    this->swap_node(node, node->right);
    Node *buffer = node->left;
    node->left = node->right;
    node->right = node->left->right;
    node->left->right = node->left->left;
    node->left->left = buffer;
}


int main()
{
    Tree tree1;
    tree1.add_node(10, "Mos");
    tree1.add_node(4, "Lon");
    tree1.add_node(15, "Par");
    tree1.add_node(16, "New");
    tree1.add_node(14, "Mad");
    tree1.add_node(2, "May");
    tree1.add_node(6, "Fan");
    tree1.add_node(3, "Gab");
    tree1.add_node(1, "Pan");
    tree1.add_node(7, "hab");
    tree1.add_node(5, "van");
    tree1.display();
    tree1.delete_node(10);
    std::cout<<tree1.root->key<<std::endl;
    tree1.display();
}