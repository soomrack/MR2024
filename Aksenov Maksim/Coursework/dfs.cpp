#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>

using namespace std;

// Проверка входных параметров
void validate_input(int num_nodes, int max_edges_per_node) {
    if (num_nodes <= 0) {
        throw invalid_argument("Количество вершин должно быть положительным.");
    }
    if (max_edges_per_node <= 0) {
        throw invalid_argument("Максимальное число рёбер на вершину должно быть положительным.");
    }
}

vector<vector<int>> generate_dag(int num_nodes, int max_edges_per_node) {
    validate_input(num_nodes, max_edges_per_node); // Валидация входных данных

    vector<vector<int>> graph(num_nodes);
    mt19937 random_generator(1234);
    uniform_int_distribution<int> node_distribution(0, num_nodes - 1);

    try {
        for (int current_node = 0; current_node < num_nodes; ++current_node) {
            int max_possible_edges = min(max_edges_per_node, num_nodes - current_node - 1);
            if (max_possible_edges <= 0) continue;

            uniform_int_distribution<int> edge_count_distribution(0, max_possible_edges);
            int edge_count = edge_count_distribution(random_generator);

            for (int edge_index = 0; edge_index < edge_count; ++edge_index) {
                int target_node = current_node + 1 + (node_distribution(random_generator) % (num_nodes - current_node - 1));
                if (target_node >= num_nodes) {
                    throw out_of_range("Некорректная генерация целевой вершины.");
                }
                graph[current_node].push_back(target_node);
            }
        }
    } catch (const exception& e) {
        cerr << "Ошибка генерации графа: " << e.what() << endl;
        throw;
    }

    return graph;
}

void dfs(const vector<vector<int>>& graph, int start_node) {
    if (start_node < 0 || start_node >= graph.size()) {
        throw out_of_range("Стартовая вершина вне диапазона графа.");
    }

    unordered_set<int> visited_nodes;
    stack<int> dfs_stack;
    dfs_stack.push(start_node);
    visited_nodes.insert(start_node);

    while (!dfs_stack.empty()) {
        int current_node = dfs_stack.top();
        dfs_stack.pop();

        for (auto neighbor_iterator = graph[current_node].rbegin(); neighbor_iterator != graph[current_node].rend(); ++neighbor_iterator) {
            int neighbor = *neighbor_iterator;
            if (neighbor < 0 || neighbor >= graph.size()) {
                throw out_of_range("Некорректная вершина в списке смежности.");
            }
            if (!visited_nodes.count(neighbor)) {
                visited_nodes.insert(neighbor);
                dfs_stack.push(neighbor);
            }
        }
    }
}

int main() {
    try {
        int num_nodes = 100000;
        int max_edges_per_node = 5;

        vector<vector<int>> graph = generate_dag(num_nodes, max_edges_per_node);

        int runs = 10;
        double total_time = 0;

        for (int run_index = 0; run_index < runs; ++run_index) {
            auto start_time = chrono::high_resolution_clock::now();
            dfs(graph, 0);
            auto end_time = chrono::high_resolution_clock::now();
            total_time += chrono::duration<double>(end_time - start_time).count();
        }

        cout << "Среднее время DFS: " << (total_time / runs) << " сек\n";
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }
    return 0;
}
