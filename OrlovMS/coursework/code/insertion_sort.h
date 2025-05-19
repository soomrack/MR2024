#pragma once
#include <vector>
#include <cstddef>
#include "comparator.h"


template<typename Element>
void insertion_sort(std::vector<Element> &array, const Comparator<Element>comp = is_greater)
{
    size_t idx_max = array.size();

    for(size_t idx = 1; idx < idx_max; idx++) {
        Element elem = array[idx];

        size_t insert_idx = idx;
        for(;insert_idx > 0 && comp(array[insert_idx - 1], elem); insert_idx--) {
            array[insert_idx] = array[insert_idx - 1];
        }
        array[insert_idx] = elem;
    }
}
