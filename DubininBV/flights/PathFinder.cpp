#include "PathFinder.h"
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <stack>

using namespace std;

typedef string city_name;
typedef string neighbour_name;
typedef string previous_city_name;
typedef int distance;
typedef int distance_from_origin;



unordered_map<city_name, unordered_map<neighbour_name, distance>> PathFinder::create_graph(const vector<Flight>& data) {
    unordered_map<city_name, unordered_map<neighbour_name, distance>> graph;

    for (Flight flight : data) {
        graph[flight.origin_city][flight.dest_city] = flight.air_time;
    }

    return graph;
}




string PathFinder::find_path(
    const string& origin, 
    const string& destination, 
    const vector<Flight>& data) {

    const unordered_map<city_name, unordered_map<neighbour_name, distance>>& graph = create_graph(data);


    unordered_map<city_name, distance_from_origin> distances;
    for (const auto& node : graph) {
        distances[node.first] = INT_MAX;
    }
    distances[origin] = 0;


    priority_queue<Vertex, vector<Vertex>, greater<Vertex>> pq;
    pq.push({origin, 0});


    unordered_map<city_name, previous_city_name prev;


    while (!pq.empty()) {
        Vertex node = pq.top();
        pq.pop();

        if (node.distance > distances[node.city_name]) continue;

        for (const auto& neighbor : graph.at(node.city_name)) {
            int new_distance = node.distance + neighbor.second;

            if (new_distance < distances[neighbor.first]) {
                distances[neighbor.first] = new_distance;
                pq.push({neighbor.first, new_distance});
                prev[neighbor.first] = node.city_name;
            }
        }
    }

    if (prev.find(destination) == prev.end()) {
        return "Path not found!";
    }

    
    stack<string> path_stack;
    for (string city = destination; city != origin; city = prev[city]) {
        path_stack.push(city);
    }


    string path;
    while(!path_stack.empty()) {
        path += path_stack.top() + " --> ";
        path_stack.pop();
    }

    return origin + path;
}
