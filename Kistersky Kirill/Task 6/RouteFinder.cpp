#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <map>
#include <stdexcept>

using namespace std;

struct airport_info {
    string code;
    string name;
    string city;
};

struct vertex {
    int prev = -1;
    int dist = numeric_limits<int>::max();
};

class flight_graph {
private:
    vector<vector<pair<int, int>>> adjacency_list;
    unordered_map<int, airport_info> id_to_airport;
    unordered_map<string, int> airport_to_id;

public:
    void load_mappings(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open mapping file " + filename);
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string code, id_str, name, city;

            getline(ss, code, ',');
            getline(ss, id_str, ',');
            getline(ss, name, ',');
            getline(ss, city);

            int id = stoi(id_str);
            id_to_airport[id] = { code, name, city };
            airport_to_id[code] = id;
        }
    }

    void load_graph(const string& filename) {
        int max_id = find_max_id();
        adjacency_list.resize(max_id + 1);

        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open graph file " + filename);
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string from_str, to_str, dist_str;
            getline(ss, from_str, ',');
            getline(ss, to_str, ',');
            getline(ss, dist_str);

            int from = stoi(from_str);
            int to = stoi(to_str);
            int dist = stoi(dist_str);

            adjacency_list[from].emplace_back(to, dist);
        }
    }

    vector<int> find_shortest_path(int start, int end) {
        if (start == end) {
            return { start };
        }

        int n = adjacency_list.size();
        vector<vertex> vertices(n);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        vertices[start].dist = 0;
        pq.push({ 0, start });

        while (!pq.empty()) {
            auto current = pq.top();
            pq.pop();
            int current_dist = current.first;
            int u = current.second;

            if (current_dist > vertices[u].dist) continue;
            if (u == end) break;

            for (const auto& edge : adjacency_list[u]) {
                int v = edge.first;
                int weight = edge.second;
                int new_dist = vertices[u].dist + weight;
                if (new_dist < vertices[v].dist) {
                    vertices[v].dist = new_dist;
                    vertices[v].prev = u;
                    pq.push({ new_dist, v });
                }
            }
        }

        if (vertices[end].dist == numeric_limits<int>::max()) {
            throw runtime_error("No path exists between " + to_string(start) + " and " + to_string(end));
        }

        vector<int> path;
        for (int v = end; v != -1; v = vertices[v].prev) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());

        return path;
    }

    void print_path(const vector<int>& path) {
        cout << "\nShortest flight path:\nRoute: ";
        for (size_t i = 0; i < path.size(); ++i) {
            const auto& airport = id_to_airport.at(path[i]);
            cout << airport.code << " (" << airport.name << ")";
            if (i != path.size() - 1) cout << " -> ";
        }
        cout << "\nTotal distance: " << get_path_distance(path) << " miles\n";
    }

    int get_path_distance(const vector<int>& path) {
        if (path.size() < 2) return 0;

        int distance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            int from = path[i];
            int to = path[i + 1];
            for (const auto& edge : adjacency_list[from]) {
                if (edge.first == to) {
                    distance += edge.second;
                    break;
                }
            }
        }
        return distance;
    }

    void print_airport_list() {
        cout << "\nAvailable airports (" << id_to_airport.size() << "):\n";
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(10) << "ID" << setw(10) << "Code" << setw(30) << "Airport Name" << setw(20) << "City" << "\n";
        cout << "----------------------------------------------------------------------\n";

        map<int, airport_info> sorted_airports(id_to_airport.begin(), id_to_airport.end());

        for (const auto& pair : sorted_airports) {
            cout << setw(10) << pair.first
                << setw(10) << pair.second.code
                << setw(30) << pair.second.name
                << setw(20) << pair.second.city << "\n";
        }

        cout << "----------------------------------------------------------------------\n";
        cout << "Example: For New York (JFK) -> Los Angeles (LAX), enter 'JFK' then 'LAX'\n";
    }

    int get_airport_id(const string& code) const {
        if (airport_to_id.count(code) == 0) {
            throw runtime_error("Invalid airport code: " + code);
        }
        return airport_to_id.at(code);
    }

private:
    int find_max_id() const {
        int max_id = 0;
        for (const auto& pair : id_to_airport) {
            if (pair.first > max_id) {
                max_id = pair.first;
            }
        }
        return max_id;
    }
};

int main() {
    try {
        cout << "=== Airline Route Finder ===\n";

        flight_graph graph;
        graph.load_mappings("airport_mapping.csv");
        graph.load_graph("airline_graph.csv");

        graph.print_airport_list();

        while (true) {
            cout << "\nEnter departure airport code (3 letters, e.g. LAX): ";
            string start_code;
            cin >> start_code;

            cout << "Enter arrival airport code (3 letters, e.g. JFK): ";
            string end_code;
            cin >> end_code;

            transform(start_code.begin(), start_code.end(), start_code.begin(), ::toupper);
            transform(end_code.begin(), end_code.end(), end_code.begin(), ::toupper);

            try {
                int start = graph.get_airport_id(start_code);
                int end = graph.get_airport_id(end_code);

                vector<int> path = graph.find_shortest_path(start, end);
                graph.print_path(path);
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << "\n";
                continue;
            }

            cout << "\nAnother search? (y/n): ";
            char choice;
            cin >> choice;
            if (tolower(choice) != 'y') break;
        }

    }
    catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
