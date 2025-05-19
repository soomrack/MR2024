#pragma once
#include <vector>
#include <cstddef>
#include <cstdlib>
#include "comparator.h"


template<typename Element>
size_t quick_sort_partition(std::vector<Element> &array, const size_t idx_start, const size_t idx_end, const Comparator<Element>comp)
{
    Element ref_element = array[idx_start + (size_t)rand() % (idx_end - idx_start + 1)];

    // Hoare scheme
    size_t idx_low = idx_start;
    size_t idx_high = idx_end;
    while(true) {
        while(comp(ref_element, array[idx_low])) idx_low++;
        while(comp(array[idx_high], ref_element)) idx_high--;

        if(idx_low >= idx_high) break;

        std::swap(array[idx_low], array[idx_high]);
        idx_low++;
        idx_high--;
    }

    return idx_high;
}


template<typename Element>
void quick_sort_internal(std::vector<Element> &array, const size_t idx_start, const size_t idx_end, const Comparator<Element>comp)
{
    if(idx_start < idx_end) {
        size_t idx_middle = quick_sort_partition(array, idx_start, idx_end, comp);
        quick_sort_internal(array, idx_start, idx_middle, comp);
        quick_sort_internal(array, idx_middle + 1, idx_end, comp);
    }
}


template<typename Element>
void quick_sort(std::vector<Element> &array, const Comparator<Element>comp = is_greater)
{
    quick_sort_internal(array, 0, array.size() - 1, comp);
}
