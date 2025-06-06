#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


struct Edge {
    std::string destination;
    int weight;
};

struct Point {
    std::string point;
    std::vector<Edge> edges;
};

std::pair<std::vector<std::string>, int> BellmanFord(const std::unordered_map<std::string, Point> graph,
const std::string& start, const std::string& end)
{
    if (graph.find(start) == graph.end() || graph.find(end) == graph.end()) {
        return { {}, std::numeric_limits<int>::max() };
    }
    
    // Инициализация расстояний и порядка прохождения
    std::unordered_map<std::string, int> distance;
    std::unordered_map<std::string, std::string> order;
    
    for (const auto& point : graph) {
        distance[point.first] = std::numeric_limits<int>::max();
    }
    distance[start] = 0;
    
    // Обработка графа для применения алгоритма
    std::vector<std::pair<std::string, Edge>> edges;
    for (const auto& point : graph) {
        for (const auto& edge : point.second.edges) {
            edges.emplace_back(point.first, edge);
        }
    }
    
    // Основной цикл алгоритма
    for (size_t idx = 0; idx < graph.size() - 1; idx++) {
        for (const auto& cur_edge : edges) {
            if (distance[cur_edge.first] != std::numeric_limits<int>::max() &&
                distance[cur_edge.first] + cur_edge.second.weight < distance[cur_edge.second.destination]) {
                distance[cur_edge.second.destination] = distance[cur_edge.first] + cur_edge.second.weight;
                order[cur_edge.second.destination] = cur_edge.first;
            }
        }
    }
    
    // Проверка на отрицательные циклы
    for (const auto& point : graph) {
        for (const auto& edge : point.second.edges) {
            if (distance[point.first] != std::numeric_limits<int>::max() &&
                distance[point.first] + edge.weight < distance[edge.destination]) {
                //throw std::runtime_error("Граф содержит отрицательный цикл");
            }
        }
    }
    
    if (distance[end] == std::numeric_limits<int>::max()) {
        return{{}, std::numeric_limits<int>::max() };
    }

    // Сборка пути
    std::vector<std::string> path;
    for (std::string at = end; at != ""; at = order[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    
    return {path, distance[end]};
}


std::pair<std::vector<std::string>, int> FloydWarshall(const std::unordered_map<std::string, Point> graph,
    const std::string& start, const std::string& end)
{
    // Обработка графа для применения алгоритма
    std::vector<std::string> points;
    
    if (graph.find(start) == graph.end() || graph.find(end) == graph.end()) {
        return {{}, std::numeric_limits<int>::max()};
    }
    
    for (const auto& point : graph) {
        points.push_back(point.first);
    }

    // Инициализация расстояний и порядка прохождения
    std::unordered_map<std::string, std::unordered_map<std::string, int>> distance;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> order;

    for (const auto& point : points) {
        for (const auto& destinatoin : points) {
            if (point == destinatoin) {distance[point][destinatoin] = 0; }
            else { distance[point][destinatoin] = std::numeric_limits<int>::max(); }
            order[point][destinatoin] = "";
        }
    }
    
    // Заполнение рёбер минимальными расстояниями
    for (const auto& point : graph) {
        for (const auto& edge : point.second.edges) {
            if (distance[point.first][edge.destination] > edge.weight) {
                distance[point.first][edge.destination] = edge.weight;
                order[point.first][edge.destination] = edge.destination;
            }
        }
    }
    
    // Основной алгоритм
    for (const auto& k : points) {
        for (const auto& i : points) {
            for (const auto& j : points) {
                if (distance[i][k] != std::numeric_limits<int>::max() &&
                    distance[k][j] != std::numeric_limits<int>::max() &&
                    distance[i][k] + distance[k][j] < distance[i][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                    order[i][j] = order[i][k];
                }
            }
        }
    }
    
    // Проверка на наличие отрицательных циклов
    if (distance[start][start] < 0 || distance[end][end] < 0) {
        throw std::runtime_error("Граф содержит отрицательный цикл");
    }
    
    if (order[start][end].empty()) {
        return {{}, std::numeric_limits<int>::max() };
    }
    
    // Сборка пути
    std::vector<std::string> path;
    for (std::string at = start; at != end; at = order[at][end]) {
        path.push_back(at);
    }
    path.push_back(end);
    
    return {path, distance[start][end]};
}