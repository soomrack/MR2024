#pragma once
#include <cstddef>

class IHeap {
public:
    virtual ~IHeap() {}

    virtual void insert(int value) = 0;
    virtual int extract_max() = 0;
    virtual int get_max() const = 0;
    virtual size_t size() const = 0;
    virtual bool is_empty() const = 0;
    virtual void merge(IHeap& other) = 0;
};
