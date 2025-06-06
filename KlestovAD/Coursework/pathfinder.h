#pragma once
#include <vector>
#include <string>
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
const std::string& start, const std::string& end);

std::pair<std::vector<std::string>, int> FloydWarshall(const std::unordered_map<std::string, Point> graph,
const std::string& start, const std::string& end);