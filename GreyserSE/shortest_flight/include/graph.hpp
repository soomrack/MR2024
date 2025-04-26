#pragma once

#include "parser.hpp"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class Dijkstra {
public:
    Dijkstra(const vector<Flight>& flights);
    vector<string> find_shortest_path(const string& origin, const string& destination, int& distance);
    bool is_city_exists(const string& city);
    
private:
    unordered_map<string, unordered_map<string, int>> graph;
    void build_graph(const vector<Flight>& flights);
};




