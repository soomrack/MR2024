/*#include <iostream>
#include <limits.h>
#include <chrono>
#include <cstdlib>  // Для rand()
#include <ctime>    // Для time()

constexpr auto MAX_VERTICES = 1000;

int graph[MAX_VERTICES][MAX_VERTICES]; // Матрица смежности графа

// Функция поиска вершины с минимальным расстоянием
int minDistance(int distance[], bool vertex_checked[], int num_vertices) {
    int min = INT_MAX;
    int min_index = -1;

    for (int v = 0; v < num_vertices; v++) {
        if (!vertex_checked[v] && distance[v] <= min) {
            min = distance[v];
            min_index = v;
        }
    }
    return min_index;
}

// Алгоритм Дейкстры
void dijkstra(int start_vertex, int num_vertices) {
    int distance[MAX_VERTICES];
    bool vertex_checked[MAX_VERTICES];
    int parent[MAX_VERTICES];

    // Инициализация
    for (int i = 0; i < num_vertices; i++) {
        distance[i] = INT_MAX;
        vertex_checked[i] = false;
        parent[i] = -1;
    }

    distance[start_vertex] = 0;
    parent[start_vertex] = start_vertex;

    // Основной цикл
    for (int count = 0; count < num_vertices - 1; count++) {
        int u = minDistance(distance, vertex_checked, num_vertices);
        if (u == -1) break;
        vertex_checked[u] = true;

        for (int v = 0; v < num_vertices; v++) {
            if (!vertex_checked[v] && graph[u][v] && distance[u] != INT_MAX
                && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

// Функция генерации случайного графа
void generate_random_graph(int num_vertices, int num_edges) {
    // Обнуляем матрицу смежности
    for (int i = 0; i < num_vertices; i++)
        for (int j = 0; j < num_vertices; j++)
            graph[i][j] = 0;

    int edges_added = 0;
    while (edges_added < num_edges) {
        int u = rand() % num_vertices;
        int v = rand() % num_vertices;
        if (u != v && graph[u][v] == 0) {
            int w = 1 + rand() % 20;  // Вес от 1 до 20
            graph[u][v] = w;
            edges_added++;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));  // Инициализация генератора случайных чисел

    std::cout << "Вершины;Рёбра;Время (мкс)\n";

    // Больше точек: от 50 до 1000 с шагом 50
    for (int num_vertices = 50; num_vertices <= 1000; num_vertices += 50) {
        int num_edges = num_vertices * (num_vertices / 4);  // Например, четверть полного графа

        generate_random_graph(num_vertices, num_edges);

        auto start = std::chrono::high_resolution_clock::now();

        dijkstra(0, num_vertices);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << num_vertices << ";" << num_edges << ";" << duration.count() << "\n";
    }

    return 0;
}*/
#include <limits.h>
#include <iostream>

constexpr auto number_of_vertices = 5;

int graph[number_of_vertices][number_of_vertices]; 

int minDistance(int distance[], bool vertex_checked[]) {
    int min = INT_MAX;
    int min_index = -1;

    for (int v = 0; v < number_of_vertices; v++) {
        if (!vertex_checked[v] && distance[v] <= min) {
            min = distance[v];
            min_index = v;
        }
    }
    return min_index;
}

void printSolution(int distance[], int start_vertex, int parent[]) {
    std::cout << "Вершина      Расстояние      Путь\n";
    for (int i = 0; i < number_of_vertices; i++) {
        if (distance[i] == INT_MAX) {
            std::cout << "  " << (i + 1) << "\t\t" << "недостижимо\n";
            continue;
        }

        std::cout << "  " << (i + 1) << "\t\t" << distance[i] << "\t    ";

        int path[number_of_vertices];
        int count = 0;
        int temp = i;

        while (temp != start_vertex) {
            path[count++] = temp;
            temp = parent[temp];
        }

        std::cout << (start_vertex + 1);
        for (int j = count - 1; j >= 0; j--) {
            std::cout << "->" << (path[j] + 1);
        }
        std::cout << "\n";
    }
}

void dijkstra(int start_vertex) {
    int distance[number_of_vertices];
    bool vertex_checked[number_of_vertices];
    int parent[number_of_vertices];

    for (int i = 0; i < number_of_vertices; i++) {
        distance[i] = INT_MAX;
        vertex_checked[i] = false;
        parent[i] = -1;
    }

    distance[start_vertex] = 0;
    parent[start_vertex] = start_vertex;

    for (int count = 0; count < number_of_vertices - 1; count++) {
        int u = minDistance(distance, vertex_checked);
        if (u == -1) break;
        vertex_checked[u] = true;

        for (int v = 0; v < number_of_vertices; v++) {
            if (!vertex_checked[v] && graph[u][v] && distance[u] != INT_MAX
                && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    printSolution(distance, start_vertex, parent);
}

int main() {
    setlocale(LC_ALL, "Russian");

    int num_edges;
    std::cout << "Введите количество рёбер: ";
    std::cin >> num_edges;

    // Обнуляем матрицу смежности
    for (int i = 0; i < number_of_vertices; i++)
        for (int j = 0; j < number_of_vertices; j++)
            graph[i][j] = 0;

    std::cout << "Введите рёбра (от до вес) — вершины нумеруются от 1:\n";
    for (int i = 0; i < num_edges; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        graph[u - 1][v - 1] = w; // уменьшаем на 1 при вводе
    }

    int start_vertex;
    std::cout << "Введите начальную вершину (нумерация от 1): ";
    std::cin >> start_vertex;

    std::cout << "\nАлгоритм Дейкстры:\n";
    dijkstra(start_vertex - 1); // уменьшаем на 1 при вызове

    return 0;
}
