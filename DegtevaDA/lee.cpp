#include <iostream>
#include <queue>
#include <chrono>
#include <cmath>

const int MAX_SIZE = 1000; 

struct Cell {
    int x, y;   
    int g;      
    int f;      

    bool operator>(const Cell& other) const {
        return f > other.f;
    }
};

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool isValid(int x, int y, int size) {
    return x >= 0 && x < size && y >= 0 && y < size;
}

void astar(int size) {
    bool visited[MAX_SIZE][MAX_SIZE] = { false };

    std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> open;

    Cell start;
    start.x = 0;
    start.y = 0;
    start.g = 0;
    start.f = heuristic(0, 0, size - 1, size - 1);
    open.push(start); 

    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    while (!open.empty()) {
        Cell current = open.top();
        open.pop();

        int x = current.x;
        int y = current.y;

        if (visited[x][y]) continue;
        visited[x][y] = true;

        if (x == size - 1 && y == size - 1) return;

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (isValid(nx, ny, size) && !visited[nx][ny]) {
                Cell next;
                next.x = nx;
                next.y = ny;
                next.g = current.g + 1; 
                next.f = next.g + heuristic(nx, ny, size - 1, size - 1);  
                open.push(next);  
            }
        }
    }
}

int main() {
    std::cout << "Размер;Время (мкс)\n";

   
    for (int size = 50; size <= 1000; size += 50) {
        auto start_time = std::chrono::high_resolution_clock::now();

        astar(size);  

        auto end_time = std::chrono::high_resolution_clock::now();

       
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

        
        std::cout << size << ";" << duration << "\n";
    }

    return 0;
}
