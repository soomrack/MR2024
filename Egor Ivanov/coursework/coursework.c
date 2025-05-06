#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

std::vector<int> t;
int n;

void init(int nn) {
    n = nn;
    t.assign(n, 0);
}

int sum(int right) {
    int result = 0;
    for (; right >= 0; right = (right & (right + 1)) - 1) result += t[right];
    return result;
}

void inc(int idx, int delta) {
    for (; idx < n; idx = (idx | (idx + 1))) t[idx] += delta;
}

int sum(int left, int right) {
    return sum(right) - sum(left - 1);
}

void init(std::vector<int> a) {
    init((int)a.size());
    for (unsigned idx = 0; idx < a.size(); idx++) inc(idx, a[idx]);
}

int main() {
    unsigned long int k = 1000 * 1000 * 1000;
        clock_t start, end;
        init(k);

        for (unsigned long int i = 0; i < k; ++i) {
            inc(i, i); 
        }

        for (int n = 1; n < 20; n += 1) {
            unsigned long int left = rand() % k;
            unsigned long int right = rand() % k;

            if (left > right) {
                std::swap(left, right);
            }

        start = clock();
        sum(left, right); 
        end = clock();

        printf("Time %.7f ", ((double)end - start) / ((double)CLOCKS_PER_SEC));
        printf("Elements %d\n", left- right);
    }
}


/*
#include <iostream>
#include <algorithm>
#include <vector>

std:: vector<int> t;
int n;

const int INF = 1000 * 1000 * 1000;

void init(int nn) {
    n = nn;
    t.assign(n, INF);
}

int getmin(int right) {
    int result = INF;
    for (; right >= 0; right = (right & (right + 1)) - 1) result = std::min(result, t[right]);
    return result;
}

void update(int idx, int new_val) {
    for (; idx < n; idx = (idx | (idx + 1))) t[idx] = std::min(t[idx], new_val);
}

void init(std::vector<int> a) {
    init((int)a.size());
    for (unsigned idx = 0; idx < a.size(); idx++) update(idx, a[idx]);
}

int main() {
    for (int k = 1;k < 1000 * 1000 * 1000 * 100;k *= 10) {
        clock_t start, end;
        start = clock();
        init(k);
        for (int i = 0; i < k; i++) { update(i, i); }
        getmin(k - 1);
        end = clock();
        printf("Time %.4f ", ((double)end - start) / ((double)CLOCKS_PER_SEC));
        printf("Elements %d\n", k);
    }
}
*/


/*
#include <iostream>
#include <algorithm>
#include <vector>

std::vector<int> t;
int n;

const int INF = -1000 * 1000 * 1000;

void init(int nn) {
    n = nn;
    t.assign(n, INF);
}

int getmax(int right) {
    int result = INF;
    for (; right >= 0; right = (right & (right + 1)) - 1) result = std::max(result, t[right]);
    return result;
}

void update(int idx, int new_val) {
    for (; idx < n; idx = (idx | (idx + 1))) t[idx] = std::max(t[idx], new_val);
}

void init(std::vector<int> a) {
    init((int)a.size());
    for (unsigned idx = 0; idx < a.size(); idx++) update(idx, a[idx]);
}

int main() {
    for (int k = 1;k < 1000 * 1000 * 1000 * 100;k *= 10) {
        clock_t start, end;
        start = clock();
        init(k);
        for (int i = 0; i < k; i++) { update(i, i); }
        getmax(k - 1);
        end = clock();
        printf("Time %.4f ", ((double)end - start) / ((double)CLOCKS_PER_SEC));
        printf("Elements %d\n", k);
    }
*/