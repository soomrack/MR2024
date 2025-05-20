#include <iostream>
#include <vector>
#include <chrono>
#include <list>
#include <algorithm>
#include <random>
#include <fstream>
#include <stdexcept>

class BinomialHeap {
private:
    struct Node {
        int value;
        std::vector<Node*> children;
        int order = 0;
        Node(int v) : value(v) {}
    };

    std::list<Node*> trees;

    Node* mergeTrees(Node* a, Node* b) {
        if (a->value < b->value) std::swap(a, b);
        a->children.push_back(b);
        a->order++;
        return a;
    }

    void consolidate() {
        std::vector<Node*> order_table(64, nullptr);

        while (!trees.empty()) {
            Node* current = trees.front();
            trees.pop_front();
            int order = current->order;

            while (order_table[order] != nullptr) {
                Node* other = order_table[order];
                order_table[order] = nullptr;
                current = mergeTrees(current, other);
                order++;
            }
            order_table[order] = current;
        }

        for (auto node : order_table) {
            if (node != nullptr) trees.push_back(node);
        }
    }

public:
    void insert(int value) {
        Node* new_node = new Node(value);
        trees.push_back(new_node);
        consolidate();
    }

    int extractMax() {
        if (trees.empty()) throw std::runtime_error("Heap is empty");

        auto max_it = std::max_element(trees.begin(), trees.end(),
            [](Node* a, Node* b) { return a->value < b->value; });

        Node* max_node = *max_it;
        trees.erase(max_it);

        for (auto child : max_node->children) {
            trees.push_back(child);
        }

        int max_value = max_node->value;
        delete max_node;
        consolidate();
        return max_value;
    }

    bool empty() const {
        return trees.empty();
    }
};

std::vector<int> generate_data(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000);
    std::vector<int> data;
    for (int i = 0; i < n; ++i) {
        data.push_back(distrib(gen));
    }
    return data;
}

template<typename Heap>
double measure_insert(Heap& heap, const std::vector<int>& data) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int val : data) {
        heap.insert(val);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

template<typename Heap>
double measure_extract(Heap& heap) {
    auto start = std::chrono::high_resolution_clock::now();
    while (!heap.empty()) {
        heap.extractMax();
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    std::vector<int> sizes = { 1000, 10000, 100000 };

    std::ofstream csv_file("results.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Error: Failed to create results.csv" << std::endl;
        return 1;
    }
    csv_file << "Size,Operation,BinomialHeap\n";

    for (int size : sizes) {
        std::vector<int> data = generate_data(size);
        BinomialHeap heap;

        try {
            double insert_time = measure_insert(heap, data);
            double extract_time = measure_extract(heap);

            csv_file << size << ",Insert," << insert_time << "\n";
            csv_file << size << ",Extract," << extract_time << "\n";
            csv_file.flush();

            std::cout << "Size " << size
                << " | Insert: " << insert_time << "s"
                << " | Extract: " << extract_time << "s"
                << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    csv_file.close();
    std::cout << "Data saved to results.csv" << std::endl;
    return 0;
}