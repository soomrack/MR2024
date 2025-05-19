#pragma once
#include <vector>
#include <cstddef>
#include "comparator.h"


template<typename Element>
void restore_heap(std::vector<Element> &array, size_t root_idx, const size_t heap_size, const Comparator<Element>comp)
{
    while(true)
    {
        size_t child_left_idx = 2 * root_idx + 1;
        size_t child_right_idx = 2 * root_idx + 2;

        size_t largest_idx = root_idx;
        if(child_left_idx < heap_size && comp(array[child_left_idx], array[largest_idx])) {
            largest_idx = child_left_idx;
        }
        if(child_right_idx < heap_size && comp(array[child_right_idx], array[largest_idx])) {
            largest_idx = child_right_idx;
        }

        if(largest_idx != root_idx) {
            std::swap(array[largest_idx], array[root_idx]);
            root_idx = largest_idx;
        }
        else break;
    }
}


template<typename Element>
void build_heap(std::vector<Element> &array, const Comparator<Element>comp)
{
    size_t heap_size = array.size();

    for(size_t idx = heap_size / 2; idx > 0; idx--) {
        restore_heap(array, idx - 1, heap_size, comp);
    }
}


template<typename Element>
void heap_sort(std::vector<Element> &array, const Comparator<Element>comp = is_greater)
{
    build_heap(array, comp);

    for(size_t idx = array.size(); idx > 0; idx--) {
        std::swap(array[0], array[idx - 1]);
        restore_heap(array, 0, idx - 1, comp);
    }
}
