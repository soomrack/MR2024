#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <stdexcept>

class MaxHeap {
private:
    std::vector<int> heap;

    void siftUp(int index) {
        while (index > 0 && heap[(index - 1) / 2] < heap[index]) {
            std::swap(heap[(index - 1) / 2], heap[index]);
            index = (index - 1) / 2;
        }
    }

    void siftDown(int index) {
        int maxIndex = index;
        int left = 2 * index + 1;
        if (left < heap.size() && heap[left] > heap[maxIndex]) {
            maxIndex = left;
        }
        int right = 2 * index + 2;
        if (right < heap.size() && heap[right] > heap[maxIndex]) {
            maxIndex = right;
        }
        if (maxIndex != index) {
            std::swap(heap[index], heap[maxIndex]);
            siftDown(maxIndex);
        }
    }

public:
    void insert(int value) {
        heap.push_back(value);
        siftUp(heap.size() - 1);
    }

    int extractMax() {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        int max = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            siftDown(0);
        }
        return max;
    }

    bool empty() const {
        return heap.empty();
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

int main() {
    std::vector<int> sizes = { 1000, 10000, 100000 };

    std::ofstream csv_file("results.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Error: Failed to create results.csv" << std::endl;
        return 1;
    }
    csv_file << "Size,Operation,MaxHeap\n";

    for (int size : sizes) {
        std::vector<int> data = generate_data(size);
        MaxHeap heap;

        try {
            // Замер вставки
            auto insert_start = std::chrono::high_resolution_clock::now();
            for (int val : data) {
                heap.insert(val);
            }
            auto insert_end = std::chrono::high_resolution_clock::now();
            double insert_time = std::chrono::duration<double>(insert_end - insert_start).count();

            // Замер извлечения
            auto extract_start = std::chrono::high_resolution_clock::now();
            while (!heap.empty()) {
                heap.extractMax();
            }
            auto extract_end = std::chrono::high_resolution_clock::now();
            double extract_time = std::chrono::duration<double>(extract_end - extract_start).count();

            // Запись в CSV
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