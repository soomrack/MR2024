#pragma once
#include <vector>
#include <cstddef>
#include "comparator.h"


template<typename Element>
std::vector<Element>& merge_sort_internal(std::vector<Element> &array, std::vector<Element> &buffer, const size_t idx_start, const size_t idx_end, const Comparator<Element>comp)
{
    if(idx_start == idx_end) {
        return array;
    }

    size_t idx_middle = idx_start + (idx_end - idx_start) / 2;

    std::vector<Element> &left_sorted = merge_sort_internal(array, buffer, idx_start, idx_middle, comp);
    std::vector<Element> &right_sorted = merge_sort_internal(array, buffer, idx_middle + 1, idx_end, comp);

    std::vector<Element> &sorted = (left_sorted == array) ? buffer : array;

    size_t idx_left = idx_start;
    size_t idx_right = idx_middle + 1;
    for(size_t idx = idx_start; idx <= idx_end; idx++) {
        if(idx_left <= idx_middle && idx_right <= idx_end) {
            if(comp(left_sorted[idx_left], right_sorted[idx_right])) {
                sorted[idx] = right_sorted[idx_right];
                idx_right++;
            }
            else {
                sorted[idx] = left_sorted[idx_left];
                idx_left++;
            }
        }
        else if(idx_left <= idx_middle) {
            sorted[idx] = left_sorted[idx_left];
            idx_left++;
        }
        else {
            sorted[idx] = right_sorted[idx_right];
            idx_right++;
        }
    }

    return sorted;
}


template<typename Element>
void merge_sort(std::vector<Element> &array, const Comparator<Element>comp = is_greater)
{
    std::vector<Element>buffer(array.size());

    std::vector<Element> &sorted = merge_sort_internal(array, buffer, 0, array.size() - 1, comp);

    if(sorted == buffer) {
        array = buffer;
    }
}
