#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include <cmath>
#include <numeric> 
#include <iterator>

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

template<typename InputIt, typename OutputIt, typename Size, typename RNG>
void manual_sample(InputIt first, InputIt last, OutputIt out, Size n, RNG&& g) {
    std::vector<typename std::iterator_traits<InputIt>::value_type> pool(first, last);
    std::shuffle(pool.begin(), pool.end(), g);

    for (Size i = 0; i < n && i < pool.size(); ++i) {
        *out++ = pool[i];
    }
}

void precise_performance_test() {
    std::vector<int> sizes = { 1000, 5000, 10000, 50000, 100000, 500000 };
    const int warmup_runs = 3;
    const int measured_runs = 5;
    const int search_tests = 10000;

    std::cout << "Precise Performance Analysis:\n";
    std::cout << std::setw(10) << "Size"
        << std::setw(15) << "Insert (ms)"
        << std::setw(15) << "Insert/n (ns)"
        << std::setw(15) << "Search (ns)"
        << std::setw(20) << "Theor O(log n)\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int size : sizes) {
        // Прогрев кэша
        for (int w = 0; w < warmup_runs; ++w) {
            AVLTree warmup_tree;
            std::vector<int> warmup_data(size);
            std::iota(warmup_data.begin(), warmup_data.end(), 1);
            std::shuffle(warmup_data.begin(), warmup_data.end(), gen);
            for (int val : warmup_data) {
                warmup_tree.insert(val);
            }
        }

        // Основные измерения
        double total_insert_time = 0;
        double total_search_time = 0;

        for (int r = 0; r < measured_runs; ++r) {
            AVLTree tree;
            std::vector<int> data(size);
            std::iota(data.begin(), data.end(), 1);
            std::shuffle(data.begin(), data.end(), gen);

            // Тест вставки
            auto insert_start = std::chrono::high_resolution_clock::now();
            for (int val : data) {
                tree.insert(val);
            }
            auto insert_end = std::chrono::high_resolution_clock::now();
            total_insert_time += std::chrono::duration_cast<std::chrono::nanoseconds>(insert_end - insert_start).count();

            // Тест поиска с ручной выборкой
            std::vector<int> search_keys;
            manual_sample(data.begin(), data.end(), std::back_inserter(search_keys),
                search_tests, gen);

            auto search_start = std::chrono::high_resolution_clock::now();
            for (int key : search_keys) {
                // tree.search(key); // Раскомментировать после реализации search()
            }
            auto search_end = std::chrono::high_resolution_clock::now();
            total_search_time += std::chrono::duration_cast<std::chrono::nanoseconds>(search_end - search_start).count();
        }

        // Расчет средних значений
        double avg_insert_time = total_insert_time / (measured_runs * size);
        double avg_search_time = total_search_time / (measured_runs * search_tests);
        double theoretical = 50 * log2(size);

        std::cout << std::setw(10) << size
            << std::setw(15) << std::fixed << std::setprecision(2) << total_insert_time / (measured_runs * 1e6)
            << std::setw(15) << std::fixed << std::setprecision(2) << avg_insert_time
            << std::setw(15) << std::fixed << std::setprecision(2) << avg_search_time
            << std::setw(20) << std::fixed << std::setprecision(2) << theoretical
            << "\n";
    }
}

int main() {
    precise_performance_test();
    return 0;
}
