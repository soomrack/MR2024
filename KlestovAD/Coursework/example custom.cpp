#include <fstream>
#include <iostream>
#include <chrono>

#include "pathfinder.h"

void print_result(std::unordered_map<std::string, Point> graph,
    const std::string& start, const std::string& end, const std::string& algoritm_name) {
    auto begin_time = std::chrono::steady_clock::now();
    std::vector<std::string> path;
    int dist;
    if (algoritm_name == "Bellman-Ford") {
        path = BellmanFord(graph, start, end).first;
        dist = BellmanFord(graph, start, end).second;
    }
    else if (algoritm_name == "Floyd-Warshall") {
        path = FloydWarshall(graph, start, end).first;
        dist = FloydWarshall(graph, start, end).second;
    }
    else {
        std::cout << "Incorrect algoritm name" << std::endl;
        return;
    }
    if (path.empty() || dist == -1) {
        std::cout << "No path found" << std::endl;
    }
    else {
        std::cout << "Optimal path:" << std::endl;
        for (size_t idx = 0; idx < path.size(); idx++) {
            if (idx != 0) std::cout << " - ";
            std::cout << graph[path[idx]].point;
        }
        std::cout << "\ntotal distance: " << dist << std::endl;
    }
    auto end_time = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    std::cout << "  algoritm time: " << time.count() << " ms\n";
}

int main() {
    std::unordered_map<std::string, Point> graph = {
        {"AMO", {"AMO", {{"HUH", 52}, {"GUS", 7}, {"IMP", 33}}}},
        {"GUS", {"GUS", {{"HUH", -17}, {"IMP", 3}}}},
        {"HUH", {"HUH", {{"SAD", -66}, {"AMO", 12}}}},
        {"SAD", {"SAD", {}}},
        {"IMP", {"IMP", {{"OST", 4}, {"AMO", 2}}}},
        {"OST", {"OST", {}}},
    };

    std::cout << "Bellman-Ford:" << std::endl;
    print_result(graph, "AMO", "OST", "Bellman-Ford");
    std::cout << "Floyd-Warshall:" << std::endl;
    print_result(graph, "AMO", "OST", "Floyd-Warshall");

    return 1;
}