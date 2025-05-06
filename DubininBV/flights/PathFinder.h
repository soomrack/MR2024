#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "Parser.h"

struct Vertex {
    std::string city_name;
    int distance;
    bool operator>(const Vertex& vertex) const;
};

class PathFinder {
public:
    std::string find_path(
        const std::string& origin, 
        const std::string& destination, 
        const std::vector<Flight>& data);
private:
    std::unordered_map<std::string, std::unordered_map<std::string, int>> 
        create_graph(const std::vector<Flight>& data);
};