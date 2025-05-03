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
#include <windows.h>
#include <climits>

using namespace std;

enum ConsoleColor {
    RED = 12,
    GREEN = 10,
    YELLOW = 14,
    BLUE = 9,
    WHITE = 15,
    CYAN = 11
};

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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
    distance_t dist = INT_MAX;
    int transfers = -1;
};

class flight_graph {
private:
    adjacency_list adjacency_list_;
    id_to_airport_map id_to_airport_;
    airport_to_id_map airport_to_id_;

    airport_id find_max_id() const {
        airport_id max_id = 0;
        for (const auto& pair : id_to_airport_) {
            if (pair.first > max_id) {
                max_id = pair.first;
            }
        }
        return max_id;
    }

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

    airport_id resolve_airport(const string& user_input) const {
        string query = user_input;
        transform(query.begin(), query.end(), query.begin(), ::tolower);

        for (const auto& pair : id_to_airport_) {
            string code = pair.second.code;
            string name = pair.second.name;
            string city = pair.second.city;

            transform(code.begin(), code.end(), code.begin(), ::tolower);
            transform(name.begin(), name.end(), name.begin(), ::tolower);
            transform(city.begin(), city.end(), city.begin(), ::tolower);

            if (code == query || name == query || city == query) {
                return pair.first;
            }
        }

        throw runtime_error("No matching airport for: " + user_input);
    }

    vector<airport_id> find_shortest_path(airport_id start, airport_id end, int max_transfers = -1) {
        if (start == end) return { start };

        size_t n = adjacency_list_.size();
        vector<vertex> vertices(n);
        priority_queue<pair<distance_t, airport_id>,
            vector<pair<distance_t, airport_id>>,
            greater<pair<distance_t, airport_id>>> pq;

        vertices[start].dist = 0;
        vertices[start].transfers = 0;
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
                int new_transfers = vertices[u].transfers + 1;

                if ((max_transfers == -1 || new_transfers <= max_transfers) &&
                    new_dist < vertices[v].dist) {
                    vertices[v].dist = new_dist;
                    vertices[v].prev = u;
                    vertices[v].transfers = new_transfers;
                    pq.push({ new_dist, v });
                }
            }
        }

        if (vertices[end].dist == INT_MAX) {
            throw runtime_error("No path exists with given constraints");
        }

        vector<airport_id> path;
        for (airport_id v = end; v != -1; v = vertices[v].prev) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());

        return path;
    }

    distance_t get_path_distance(const vector<airport_id>& path) const {
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

    void print_path(const vector<airport_id>& path) const {
        setColor(CYAN);
        cout << "\n+==========================================+";
        cout << "\n|          SHORTEST FLIGHT PATH            |";
        cout << "\n+==========================================+\n";
        setColor(WHITE);

        setColor(YELLOW);
        cout << "\nRoute: ";
        setColor(WHITE);
        for (size_t i = 0; i < path.size(); ++i) {
            const airport_info& airport = id_to_airport_.at(path[i]);
            setColor(GREEN);
            cout << airport.code;
            setColor(WHITE);
            cout << " (" << airport.name << ")";
            if (i != path.size() - 1) {
                setColor(RED);
                cout << " -> ";
                setColor(WHITE);
            }
        }

        setColor(YELLOW);
        cout << "\n\nDetails:\n";
        setColor(WHITE);
        for (size_t i = 0; i < path.size() - 1; ++i) {
            airport_id from = path[i];
            airport_id to = path[i + 1];
            distance_t dist = 0;

            for (const auto& edge : adjacency_list_[from]) {
                if (edge.first == to) {
                    dist = edge.second;
                    break;
                }
            }

            cout << "  " << id_to_airport_.at(from).code << " -> "
                << id_to_airport_.at(to).code << ": " << dist << " miles ("
                << id_to_airport_.at(from).city << " to " << id_to_airport_.at(to).city << ")\n";
        }

        setColor(YELLOW);
        cout << "\nTotal distance: ";
        setColor(GREEN);
        cout << get_path_distance(path) << " miles\n";
        setColor(WHITE);
    }

    void search_airports(const string& query) const {
        setColor(CYAN);
        cout << "\nSearch results for '" << query << "':\n";
        setColor(WHITE);
        cout << "----------------------------------------\n";

        bool found = false;
        string query_lower = query;
        transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);

        for (const auto& pair : id_to_airport_) {
            const airport_info& airport = pair.second;
            string code = airport.code;
            string name = airport.name;
            string city = airport.city;

            transform(code.begin(), code.end(), code.begin(), ::tolower);
            transform(name.begin(), name.end(), name.begin(), ::tolower);
            transform(city.begin(), city.end(), city.begin(), ::tolower);

            if (code.find(query_lower) != string::npos ||
                name.find(query_lower) != string::npos ||
                city.find(query_lower) != string::npos) {
                found = true;
                cout << "  ";
                setColor(GREEN);
                cout << airport.code;
                setColor(WHITE);
                cout << " - " << airport.name << ", " << airport.city << "\n";
            }
        }

        if (!found) {
            setColor(RED);
            cout << "  No airports found matching your query\n";
            setColor(WHITE);
        }
        cout << "----------------------------------------\n";
    }

    void print_airport_list() const {
        setColor(CYAN);
        cout << "\nAvailable airports:\n";
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(10) << "ID" << setw(10) << "Code"
            << setw(30) << "Airport Name" << setw(20) << "City" << "\n";
        cout << "----------------------------------------------------------------------\n";
        setColor(WHITE);

        map<airport_id, airport_info> sorted(id_to_airport_.begin(), id_to_airport_.end());
        for (const auto& pair : sorted) {
            cout << setw(10) << pair.first
                << setw(10) << pair.second.code
                << setw(30) << pair.second.name
                << setw(20) << pair.second.city << "\n";
        }

        setColor(CYAN);
        cout << "----------------------------------------------------------------------\n";
        setColor(WHITE);
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    try {
        setColor(CYAN);
        cout << "+==========================================+";
        cout << "\n|        AIRLINE ROUTE FINDER v2.0        |";
        cout << "\n+==========================================+\n\n";
        setColor(WHITE);

        flight_graph graph;
        graph.load_mappings("airport_mapping.csv");
        graph.load_graph("airline_graph.csv");

        while (true) {
            setColor(YELLOW);
            cout << "\nMain menu:\n";
            setColor(WHITE);
            cout << "1. Find shortest path between airports\n";
            cout << "2. Search airports by name/city/code\n";
            cout << "3. View all airports\n";
            cout << "4. Exit\n";
            cout << "Choose option: ";

            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                while (true) {
                    string start_input, end_input;
                    int max_transfers = -1;

                    cout << "\nEnter departure airport (code, name, or city) or 'back': ";
                    getline(cin, start_input);
                    if (start_input == "back") break;

                    cout << "Enter arrival airport (code, name, or city) or 'back': ";
                    getline(cin, end_input);
                    if (end_input == "back") break;

                    cout << "Max number of transfers (-1 for no limit): ";
                    cin >> max_transfers;
                    cin.ignore();

                    try {
                        airport_id start = graph.resolve_airport(start_input);
                        airport_id end = graph.resolve_airport(end_input);

                        vector<airport_id> path = graph.find_shortest_path(start, end, max_transfers);
                        graph.print_path(path);
                    }
                    catch (const exception& e) {
                        setColor(RED);
                        cout << "Error: " << e.what() << "\n";
                        setColor(WHITE);
                    }

                    cout << "\nAnother search? (y/n): ";
                    char again;
                    cin >> again;
                    cin.ignore();
                    if (tolower(again) != 'y') break;
                }
            }
            else if (choice == 2) {
                string query;
                cout << "\nEnter search query (name, city or code): ";
                getline(cin, query);
                graph.search_airports(query);
            }
            else if (choice == 3) {
                graph.print_airport_list();
            }
            else if (choice == 4) {
                break;
            }
            else {
                setColor(RED);
                cout << "Invalid choice, try again\n";
                setColor(WHITE);
            }
        }

        setColor(GREEN);
        cout << "\nThank you for using Airline Route Finder!\n";
        setColor(WHITE);
    }
    catch (const exception& e) {
        setColor(RED);
        cerr << "Fatal error: " << e.what() << endl;
        setColor(WHITE);
        return 1;
    }

    return 0;
}
