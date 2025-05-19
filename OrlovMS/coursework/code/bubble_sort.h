#pragma once
#include <vector>
#include <cstddef>
#include "comparator.h"


template<typename Element>
void bubble_sort(std::vector<Element> &array, const Comparator<Element>comp = is_greater)
{
    size_t itr_num = array.size() - 1;  // iterations number

    for(size_t itr = 0; itr < itr_num; itr++) {
        bool is_sorted = true;

        for(size_t idx = 0; idx < itr_num; idx++) {
            if(comp(array[idx], array[idx + 1])) {
                std::swap(array[idx], array[idx + 1]);
                is_sorted = false;
            }
        }

        if(is_sorted) break;
    }
}
