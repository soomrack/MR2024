#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <stdexcept>

using namespace std;

struct Route {
    string destination;
    int airtime;
};

unordered_map<string, vector<Route>> flight_graph;

void load_graph(const string& file_path) {
    flight_graph.clear();
    
    ifstream input_file(file_path);
    if (!input_file.is_open()) {
        throw runtime_error("Error: Could not open file: " + file_path);
    }

    string header_line;
    getline(input_file, header_line); 

    string data_line;
    while (getline(input_file, data_line)) {
        stringstream ss(data_line);
        string origin, dest, airtime_str;
        
        getline(ss, origin, ',');
        getline(ss, dest, ',');
        getline(ss, airtime_str);

        try {
            flight_graph[origin].push_back({dest, stoi(airtime_str)}); 
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
            continue;
        }
    }
}

void find_shortest_path(const string& start_airport, const string& end_airport) {

    unordered_map<string, int> total_time;
    unordered_map<string, string> previous;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> airport_queue;


    for (const auto& [airport, _] : flight_graph) {
        total_time[airport] = numeric_limits<int>::max();
    }
    total_time[start_airport] = 0;
    airport_queue.push({0, start_airport});


    while (!airport_queue.empty()) {
        auto [current_time, current_airport] = airport_queue.top();
        airport_queue.pop();

        if (current_airport == end_airport) break;
        if (current_time > total_time[current_airport]) continue;

        for (const Route& route : flight_graph[current_airport]) {
            int new_time = current_time + route.airtime;
            if (new_time < total_time[route.destination]) {
                total_time[route.destination] = new_time;
                previous[route.destination] = current_airport;
                airport_queue.push({new_time, route.destination});
            }
        }
    }

    if (total_time[end_airport] == numeric_limits<int>::max()) {
        throw runtime_error("No path exists between these airports");
    }

    vector<string> path;
    for (string airport = end_airport; !airport.empty(); airport = previous[airport]) {
        path.push_back(airport); 
    }
    reverse(path.begin(), path.end());


    cout << "\nShortest path (" << total_time[end_airport] << " minutes):\n";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i != path.size() - 1) cout << " -> ";
    }
    cout << endl;
}

int main() {
    try {
        load_graph("data.csv");
        
        string start, end;
        cout << "Enter origin airport code (3 Letters - BSQ): ";
        cin >> start;
        cout << "Enter destination airport code (3 Letters - MSC): ";
        cin >> end;


        transform(start.begin(), start.end(), start.begin(), ::toupper);
        transform(end.begin(), end.end(), end.begin(), ::toupper);

        
        if (!flight_graph.count(start)) {
            throw runtime_error("Invalid origin airport code: " + start);
        }
        if (!flight_graph.count(end)) {
            throw runtime_error("Invalid destination airport code: " + end);
        }

        find_shortest_path(start, end);
        return 0;

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }
}
