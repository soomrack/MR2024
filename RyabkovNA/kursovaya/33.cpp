#include <iostream>
#include <vector>
#include <chrono>
#include <list>
#include <random>
#include <fstream>
#include <stdexcept>
#include <cmath>

class FibonacciHeap {
private:
    struct Node {
        int value;
        std::list<Node*>::iterator self;
        std::list<Node*> children;
        Node(int v) : value(v) {}
    };

    std::list<Node*> roots;
    Node* max_node = nullptr;
    size_t size = 0;

public:
    void insert(int value) {
        try {
            Node* new_node = new Node(value);
            roots.push_back(new_node);
            new_node->self = --roots.end();
            size++;

            if (!max_node || value > max_node->value) {
                max_node = new_node;
            }
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed in insert: " << e.what() << std::endl;
            throw;
        }
    }

    int extractMax() {
        if (roots.empty()) {
            throw std::runtime_error("Heap is empty");
        }

        Node* old_max = max_node;
        roots.erase(old_max->self);
        size--;

        try {
            for (Node* child : old_max->children) {
                roots.push_back(child);
                child->self = --roots.end();
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error during child nodes processing: " << e.what() << std::endl;
            throw;
        }

        int max_value = old_max->value;
        delete old_max;

        // Простой поиск нового максимума
        max_node = nullptr;
        for (auto& node : roots) {
            if (!max_node || node->value > max_node->value) {
                max_node = node;
            }
        }

        return max_value;
    }

    bool empty() const {
        return roots.empty();
    }

    ~FibonacciHeap() {
        for (Node* node : roots) {
            delete node;
        }
    }
};

std::vector<int> generate_data(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000);
    std::vector<int> data(n);
    for (int& val : data) {
        val = distrib(gen);
    }
    return data;
}

void run_test(int size, std::ofstream& csv_file) {
    std::cout << "Running test for size: " << size << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<int> data = generate_data(size);
    auto gen_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_time).count();

    std::cout << "  Data generated in " << gen_time << " ms" << std::endl;

    FibonacciHeap heap;
    size_t successful_inserts = 0;

    // Тест вставки с прогресс-баром
    std::cout << "  Inserting elements: ";
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < size; i++) {
        try {
            heap.insert(data[i]);
            successful_inserts++;

            // Выводим прогресс каждые 10%
            if (size > 10000 && i % (size / 10) == 0) {
                std::cout << (i * 100 / size) << "% " << std::flush;
            }
        }
        catch (...) {
            std::cout << "\nFailed to insert element at position " << i << std::endl;
            throw;
        }
    }
    auto insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_time).count();
    std::cout << "100% (" << successful_inserts << " elements)" << std::endl;

    // Тест извлечения с прогресс-баром
    std::cout << "  Extracting elements: ";
    start_time = std::chrono::high_resolution_clock::now();
    size_t extracted_count = 0;
    while (!heap.empty()) {
        try {
            heap.extractMax();
            extracted_count++;

            // Выводим прогресс каждые 10%
            if (size > 10000 && extracted_count % (size / 10) == 0) {
                std::cout << (extracted_count * 100 / size) << "% " << std::flush;
            }
        }
        catch (...) {
            std::cout << "\nFailed to extract element at position " << extracted_count << std::endl;
            throw;
        }
    }
    auto extract_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_time).count();
    std::cout << "100% (" << extracted_count << " elements)" << std::endl;

    // Запись результатов
    csv_file << size << ","
        << gen_time << ","
        << insert_time << ","
        << extract_time << ","
        << (size * 1000.0 / insert_time) << ","
        << (size * 1000.0 / extract_time) << "\n";
    csv_file.flush();

    std::cout << "  Test completed for size " << size << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
}

int main() {
    const std::vector<int> sizes = { 10000, 50000, 100000, 500000, 1000000 };

    std::ofstream csv_file("results.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Failed to open results.csv" << std::endl;
        return 1;
    }

    csv_file << "Size,GenTime(ms),InsertTime(ms),ExtractTime(ms),InsertOpsPerSec,ExtractOpsPerSec\n";
    std::cout << "Performance test started...\n";
    std::cout << "----------------------------------------\n";

    for (int size : sizes) {
        try {
            run_test(size, csv_file);
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed for size " << size << ": " << e.what() << std::endl;
            std::cerr << "Skipping larger sizes..." << std::endl;
            break;
        }
        catch (const std::exception& e) {
            std::cerr << "Error with size " << size << ": " << e.what() << std::endl;
        }
    }

    csv_file.close();
    std::cout << "Testing completed. Results saved to results.csv" << std::endl;
    return 0;
}