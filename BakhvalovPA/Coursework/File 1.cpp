#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>

struct node {
    int key;
    unsigned short height;
    node* left;
    node* right;
    node(int k) : key(k), height(1), left(nullptr), right(nullptr) {}
};

class AVLTree {
private:
    node* root;

    // Вспомогательные функции
    unsigned short height(node* p) {
        return p ? p->height : 0;
    }

    int8_t bfactor(node* p) {
        return static_cast<int8_t>(height(p->right) - height(p->left));
    }

    void fixheight(node* p) {
        unsigned short hleft = height(p->left);
        unsigned short hright = height(p->right);
        p->height = (std::max(hleft, hright)) + 1;
    }

    // Балансировка
    node* rotateRight(node* p) {
        node* q = p->left;
        p->left = q->right;
        q->right = p;
        fixheight(p);
        fixheight(q);
        return q;
    }

    node* rotateLeft(node* q) {
        node* p = q->right;
        q->right = p->left;
        p->left = q;
        fixheight(q);
        fixheight(p);
        return p;
    }

    node* balance(node* p) {
        fixheight(p);
        if (bfactor(p) == 2) {
            if (bfactor(p->right) < 0)
                p->right = rotateRight(p->right);
            return rotateLeft(p);
        }
        if (bfactor(p) == -2) {
            if (bfactor(p->left) > 0)
                p->left = rotateLeft(p->left);
            return rotateRight(p);
        }
        return p;
    }

    // Вставка и удаление
    node* insert(node* p, int k) {
        if (!p) return new node(k);
        if (k < p->key)
            p->left = insert(p->left, k);
        else
            p->right = insert(p->right, k);
        return balance(p);
    }

    node* findmin(node* p) {
        return p->left ? findmin(p->left) : p;
    }

    node* removemin(node* p) {
        if (!p->left) return p->right;
        p->left = removemin(p->left);
        return balance(p);
    }

    node* remove(node* p, int k) {
        if (!p) return nullptr;
        if (k < p->key)
            p->left = remove(p->left, k);
        else if (k > p->key)
            p->right = remove(p->right, k);
        else {
            node* q = p->left;
            node* r = p->right;
            delete p;
            if (!r) return q;
            node* min = findmin(r);
            min->right = removemin(r);
            min->left = q;
            return balance(min);
        }
        return balance(p);
    }

    // Обходы дерева
    void inorder(node* p) {
        if (p) {
            inorder(p->left);
            std::cout << p->key << " ";
            inorder(p->right);
        }
    }

    void printTree(node* p, int indent = 0) {
        if (p) {
            if (p->right) printTree(p->right, indent + 4);
            if (indent) std::cout << std::setw(indent) << ' ';
            std::cout << p->key << "\n";
            if (p->left) printTree(p->left, indent + 4);
        }
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(int k) {
        root = insert(root, k);
    }

    void remove(int k) {
        root = remove(root, k);
    }

    void printSorted() {
        inorder(root);
        std::cout << std::endl;
    }

    void print() {
        printTree(root);
    }
};

std::vector<int> generate_random_data(int size) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, size * 10);

    for (int i = 0; i < size; ++i) {
        data[i] = dist(gen);
    }
    return data;
}

void performance_test() {
    std::vector<int> sizes = { 1000, 10000, 50000, 100000, 500000 };

    std::cout << "Performance test (insertion time):\n";
    std::cout << std::setw(10) << "Size" << std::setw(15) << "Time (ms)" << std::setw(15) << "Time/Item (ns)\n";

    for (int size : sizes) {
        auto data = generate_random_data(size);
        AVLTree tree;

        auto start = std::chrono::high_resolution_clock::now();

        for (int val : data) {
            tree.insert(val);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double time_per_item = duration.count() * 1e6 / size; // наносекунды на операцию

        std::cout << std::setw(10) << size
            << std::setw(15) << duration.count()
            << std::setw(15) << std::fixed << std::setprecision(2) << time_per_item
            << "\n";
    }
}

int main() {
    // Демонстрация работы с маленьким деревом
    AVLTree tree;
    int values[] = { 10, 20, 30, 40, 50, 25, 15, 5, 35, 45 };
    int n = sizeof(values) / sizeof(values[0]);

    std::cout << "Small tree demo:\n";
    std::cout << "Inserting values: ";
    for (int i = 0; i < n; i++) {
        std::cout << values[i] << " ";
        tree.insert(values[i]);
    }
    std::cout << "\n\n";

    std::cout << "Sorted values: ";
    tree.printSorted();
    std::cout << "\nTree structure:\n";
    tree.print();
    std::cout << "\n";

    // Запуск теста производительности
    performance_test();

    return 0;
}
