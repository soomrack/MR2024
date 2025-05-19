#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bubble_sort.h"
#include "heap_sort.h"
#include "insertion_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"


const size_t TEST_LEN[] = {5, 10, 20, 50, 100, 200, 500, 1000, 5000, 10000, 20000};
const int TEST_NUM = sizeof(TEST_LEN) / sizeof(size_t);


int main()
{
    std::cout << "Data length, time (us)" << std::endl;

    for(int num = 0; num < TEST_NUM; num++) {
        size_t len = TEST_LEN[num];
        std::vector<int> arr(len);

        for(size_t idx = 0; idx < len; idx++) {
            arr[idx] = std::rand();
        }

        clock_t clock_start = clock();

        // bubble_sort(arr);
        // heap_sort(arr);
        // insertion_sort(arr);
        // merge_sort(arr);
        quick_sort(arr);

        clock_t clock_end = clock();

        std::cout << len << ", " << clock_end - clock_start << std::endl;
    }

    return 0;
}
