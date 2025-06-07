#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <string>

#include "IHeap.h"
#include "max_heap.h"
#include "fibonacci_heap.h"
#include "binomial_heap.h"

using namespace std;
using namespace std::chrono;

// Размеры входных данных
const vector<size_t> TEST_SIZES = {100, 500, 1000, 5000, 10000, 20000};
constexpr double EXTRACT_FRACTION = 0.1;

// Генерация случайного массива целых чисел
vector<int> generate_data(size_t size) {
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);
    for (size_t i = 0; i < size; ++i)
        data[i] = dis(gen);
    return data;
}

// Измеряет время выполнения одной операции
template <typename Func>
long long measure_time_ms(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

// Проводит эксперимент над одной кучей
void benchmark_heap(const string& heap_name, function<unique_ptr<IHeap>()> heap_factory) {
    for (size_t size : TEST_SIZES) {
        vector<int> data = generate_data(size);
        vector<int> data2 = generate_data(size); // Для merge

        // Вставка
        auto heap = heap_factory();
        long long insert_time = measure_time_ms([&]() {
            for (int val : data)
                heap->insert(val);
        });

        // Извлечение 10% максимальных
        size_t extract_count = static_cast<size_t>(size * EXTRACT_FRACTION);
        long long extract_time = measure_time_ms([&]() {
            for (size_t i = 0; i < extract_count && !heap->is_empty(); ++i)
                heap->extract_max();
        });

        // Объединение двух куч
        auto heapA = heap_factory();
        auto heapB = heap_factory();
        for (int val : data)
            heapA->insert(val);
        for (int val : data2)
            heapB->insert(val);

        long long merge_time;
        try {
            merge_time = measure_time_ms([&]() {
                heapA->merge(*heapB);
            });
        } catch (const exception& e) {
            merge_time = -1; // Обозначим как неподдерживаемое
        }

        // CSV-вывод
        cout << heap_name << "," << size << ",insert," << insert_time << endl;
        cout << heap_name << "," << size << ",extract_max," << extract_time << endl;
        cout << heap_name << "," << size << ",merge," << merge_time << endl;
    }
}

int main() {
    cout << "Heap,Size,Operation,Time(ms)" << endl;

    benchmark_heap("MaxHeap", []() { return make_unique<MaxHeap>(); });
    benchmark_heap("FibonacciHeap", []() { return make_unique<FibonacciHeap>(); });
    benchmark_heap("BinomialHeap", []() { return make_unique<BinomialHeap>(); });

    return 0;
}
