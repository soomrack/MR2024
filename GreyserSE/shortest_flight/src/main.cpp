#include <iostream>
#include <vector>
#include <string>
#include "graph.hpp"
#include "parser.hpp"

using namespace std;

int main() {
    const string filename = "C:/C_programming/shortest_flight/T_T100_SEGMENT_ALL_CARRIER.csv";

    Parser parser;
    vector<Flight> flights = parser.parse(filename);

    Dijkstra dijkstra(flights);

    string origin, destination;
    cout << "Enter origin city: ";
    getline(cin, origin);

    while (!dijkstra.is_city_exists(origin)) {
        cout << "City not found. Please enter a valid origin city: ";
        getline(cin, origin);
    }

    cout << "Enter destination city: ";
    getline(cin, destination);

    while (!dijkstra.is_city_exists(destination)) {
        cout << "City not found. Please enter a valid destination city: ";
        getline(cin, destination);
    }

    vector<string> path;
    int air_time = 0;
    path = dijkstra.find_shortest_path(origin, destination, air_time);

    if (air_time == 0) {
        cout << "No path found from " << origin << " to " << destination << endl;
    } else {
        cout << "Shortest path from " << origin << " to " << destination << " is: " << endl;
        cout << path[0];
        for (int i = 1; i < path.size(); i++) {
            cout << " -> " << path[i];
        }
        cout << endl;
        cout << "Flight time: " << air_time / 60 << ":" << air_time % 60 << endl;
    }

    return 0;
}









