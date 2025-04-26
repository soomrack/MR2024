#include "graph.hpp"
#include <limits>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

Dijkstra::Dijkstra(const vector<Flight>& flights) {
    build_graph(flights);
}

void Dijkstra::build_graph(const vector<Flight>& flights) {
    for (const Flight& flight : flights) {
        graph[flight.origin_city][flight.dest_city] = flight.air_time;
        graph[flight.dest_city][flight.origin_city] = flight.air_time;
    }
}

vector<string> Dijkstra::find_shortest_path(const string& origin, const string& destination, int& air_time) {
    unordered_map<string, int> time;
    unordered_map<string, string> prev;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> pq;

    for (const auto& pair : graph) {
        time[pair.first] = numeric_limits<int>::max();
    }

    time[origin] = 0;
    pq.push({0, origin});

    while (!pq.empty()) {
        auto [current_time, current] = pq.top();
        pq.pop();

        if (current == destination) break;

        for (const auto& [neighbor, time_] : graph[current]) {
            if (time[neighbor] > current_time + time_) {
                time[neighbor] = current_time + time_;
                prev[neighbor] = current;
                pq.push({time[neighbor], neighbor});
            }
        }
    }

    if (time[destination] == numeric_limits<int>::max()) {
        air_time = 0;
        return {};
    }

    vector<string> path;
    for (string at = destination; at != ""; at = prev[at]) {
        path.push_back(at);
    }

    reverse(path.begin(), path.end());

    air_time = time[destination];
    return path;
}


bool Dijkstra::is_city_exists(const string& city)
{
    return graph.find(city) != graph.end();
}