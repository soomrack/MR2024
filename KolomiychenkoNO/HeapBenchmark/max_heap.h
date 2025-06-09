#pragma once
#include "IHeap.h"
#include <vector>
#include <stdexcept>
#include <utility>

class MaxHeap : public IHeap {
private:
    std::vector<int> data;

    void sift_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (data[index] <= data[parent]) break;
            std::swap(data[index], data[parent]);
            index = parent;
        }
    }

    void sift_down(size_t index) {
        size_t size = data.size();
        while (2 * index + 1 < size) {
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            size_t largest = index;

            if (left < size && data[left] > data[largest]) largest = left;
            if (right < size && data[right] > data[largest]) largest = right;
            if (largest == index) break;

            std::swap(data[index], data[largest]);
            index = largest;
        }
    }

public:
    void insert(int value) override {
        data.push_back(value);
        sift_up(data.size() - 1);
    }

    int extract_max() override {
        if (data.empty()) throw std::runtime_error("Heap is empty");
        int max_value = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) sift_down(0);
        return max_value;
    }

    int get_max() const override {
        if (data.empty()) throw std::runtime_error("Heap is empty");
        return data[0];
    }

    size_t size() const override {
        return data.size();
    }

    bool is_empty() const override {
        return data.empty();
    }

    void merge(IHeap& /*other*/) override {
        throw std::runtime_error("MaxHeap does not support merge operation");
    }
};
