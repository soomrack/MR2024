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


typedef string airport_code;
typedef int airport_id;
typedef int distance_t;


struct airport_info {
    airport_code code;
    string name;
    string city;
};

typedef pair<airport_id, distance_t> edge;
typedef vector<vector<edge>> adjacency_list;
typedef unordered_map<airport_id, airport_info> id_to_airport_map;
typedef unordered_map<airport_code, airport_id> airport_to_id_map;

struct vertex {
    airport_id prev = -1;
    distance_t dist = numeric_limits<distance_t>::max();
};

class flight_graph {
private:
    adjacency_list adjacency_list_;
    id_to_airport_map id_to_airport_;
    airport_to_id_map airport_to_id_;

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

            airport_id id = stoi(id_str);
            id_to_airport_[id] = { code, name, city };
            airport_to_id_[code] = id;
        }
    }

    void load_graph(const string& filename) {
        airport_id max_id = find_max_id();
        adjacency_list_.resize(max_id + 1);

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

            airport_id from = stoi(from_str);
            airport_id to = stoi(to_str);
            distance_t dist = stoi(dist_str);

            adjacency_list_[from].emplace_back(to, dist);
        }
    }

    vector<airport_id> find_shortest_path(airport_id start, airport_id end) {
        if (start == end) {
            return { start };
        }

        size_t n = adjacency_list_.size();
        vector<vertex> vertices(n);
        priority_queue<pair<distance_t, airport_id>,
            vector<pair<distance_t, airport_id>>,
            greater<pair<distance_t, airport_id>>> pq;

        vertices[start].dist = 0;
        pq.push({ 0, start });

        while (!pq.empty()) {
            auto current = pq.top();
            pq.pop();
            distance_t current_dist = current.first;
            airport_id u = current.second;

            if (current_dist > vertices[u].dist) continue;
            if (u == end) break;

            for (const auto& edge : adjacency_list_[u]) {
                airport_id v = edge.first;
                distance_t weight = edge.second;
                distance_t new_dist = vertices[u].dist + weight;
                if (new_dist < vertices[v].dist) {
                    vertices[v].dist = new_dist;
                    vertices[v].prev = u;
                    pq.push({ new_dist, v });
                }
            }
        }

        if (vertices[end].dist == numeric_limits<distance_t>::max()) {
            throw runtime_error("No path exists between " + to_string(start) + " and " + to_string(end));
        }

        vector<airport_id> path;
        for (airport_id v = end; v != -1; v = vertices[v].prev) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());

        return path;
    }

    void print_path(const vector<airport_id>& path) {
        cout << "\nShortest flight path:\nRoute: ";
        for (size_t i = 0; i < path.size(); ++i) {
            const auto& airport = id_to_airport_.at(path[i]);
            cout << airport.code << " (" << airport.name << ")";
            if (i != path.size() - 1) cout << " -> ";
        }
        cout << "\nTotal distance: " << get_path_distance(path) << " miles\n";
    }

    distance_t get_path_distance(const vector<airport_id>& path) {
        if (path.size() < 2) return 0;

        distance_t distance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            airport_id from = path[i];
            airport_id to = path[i + 1];
            for (const auto& edge : adjacency_list_[from]) {
                if (edge.first == to) {
                    distance += edge.second;
                    break;
                }
            }
        }
        return distance;
    }

    void print_airport_list() {
        cout << "\nAvailable airports (" << id_to_airport_.size() << "):\n";
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(10) << "ID" << setw(10) << "Code" << setw(30) << "Airport Name" << setw(20) << "City" << "\n";
        cout << "----------------------------------------------------------------------\n";

        map<airport_id, airport_info> sorted_airports(id_to_airport_.begin(), id_to_airport_.end());

        for (const auto& pair : sorted_airports) {
            cout << setw(10) << pair.first
                << setw(10) << pair.second.code
                << setw(30) << pair.second.name
                << setw(20) << pair.second.city << "\n";
        }

        cout << "----------------------------------------------------------------------\n";
        cout << "Example: For New York (JFK) -> Los Angeles (LAX), enter 'JFK' then 'LAX'\n";
    }

    airport_id get_airport_id(const airport_code& code) const {
        if (airport_to_id_.count(code) == 0) {
            throw runtime_error("Invalid airport code: " + code);
        }
        return airport_to_id_.at(code);
    }

private:
    airport_id find_max_id() const {
        airport_id max_id = 0;
        for (const auto& pair : id_to_airport_) {
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
            airport_code start_code;
            cin >> start_code;

            cout << "Enter arrival airport code (3 letters, e.g. JFK): ";
            airport_code end_code;
            cin >> end_code;

            transform(start_code.begin(), start_code.end(), start_code.begin(), ::toupper);
            transform(end_code.begin(), end_code.end(), end_code.begin(), ::toupper);

            try {
                airport_id start = graph.get_airport_id(start_code);
                airport_id end = graph.get_airport_id(end_code);

                vector<airport_id> path = graph.find_shortest_path(start, end);
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
