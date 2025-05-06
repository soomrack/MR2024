#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

using namespace std;

class flight {
public:
    string destination;
    double flight_time;

    flight(const string& dest, double time) : destination(dest), flight_time(time) {}
};

class node {
public:
    string airport_code;
    double travel_time;

    node(const string& airport, double time) : airport_code(airport), travel_time(time) {}

    bool operator>(const node& other) const {
        return travel_time > other.travel_time;
    }
};

class flight_router {
private:
    unordered_map<string, vector<flight>> flight_graph;
    unordered_map<string, string> airport_info;

    void load_from_csv(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file!\n";
            return;
        }

        string line;
        if (!getline(file, line)) {
            cerr << "Error reading header line!\n";
            return;
        }

        stringstream header_stream(line);
        unordered_map<string, int> column_index;
        string column;
        int index = 0;

        while (getline(header_stream, column, ',')) {
            column_index[column] = index++;
        }

        if (column_index.find("ORIGIN") == column_index.end() ||
            column_index.find("DEST") == column_index.end() ||
            column_index.find("AIR_TIME") == column_index.end() ||
            column_index.find("ORIGIN_CITY_NAME") == column_index.end() ||
            column_index.find("DEST_CITY_NAME") == column_index.end()) {
            cerr << "Required columns not found in CSV file!\n";
            return;
        }

        int origin_idx = column_index["ORIGIN"];
        int dest_idx = column_index["DEST"];
        int air_time_idx = column_index["AIR_TIME"];
        int origin_city_idx = column_index["ORIGIN_CITY_NAME"];
        int dest_city_idx = column_index["DEST_CITY_NAME"];

        while (getline(file, line)) {
            vector<string> row;
            stringstream ss(line);
            string cell;
            bool inside_quotes = false;
            string current_value;

            for (char c : line) {
                if (c == '"') inside_quotes = !inside_quotes;
                else if (c == ',' && !inside_quotes) {
                    row.push_back(current_value);
                    current_value.clear();
                }
                else current_value += c;
            }
            if (!current_value.empty()) row.push_back(current_value);

            if (row.size() > max({ origin_idx, dest_idx, air_time_idx,
                                origin_city_idx, dest_city_idx })) {
                string origin = row[origin_idx];
                string dest = row[dest_idx];
                double air_time = stod(row[air_time_idx]);
                string origin_city = row[origin_city_idx];
                string dest_city = row[dest_city_idx];

                if (air_time > 0) {
                    flight_graph[origin].push_back(flight(dest, air_time));
                    airport_info[origin] = origin_city;
                    airport_info[dest] = dest_city;
                }
            }
        }
    }

    vector<pair<string, string>> reconstruct_path(const string& start, const string& end,
        const unordered_map<string, string>& prev, double total_time) {
        vector<pair<string, string>> path;
        string curr = end;

        while (!curr.empty()) {
            path.emplace_back(curr, airport_info[curr]);
            if (curr == start) break;
            curr = prev.at(curr);
        }

        if (path.back().first != start) return { {"No route found", ""} };

        reverse(path.begin(), path.end());
        cout << "Total time: " << total_time << " min ("
            << total_time / 60 << " hr)\n";
        return path;
    }

public:
    flight_router(const string& filename) {
        load_from_csv(filename);
    }

    vector<pair<string, string>> find_fastest_route(const string& start, const string& end) {
        if (!flight_graph.count(start) || !flight_graph.count(end))
            return { {"Airport not found", ""} };

        unordered_map<string, double> distance_map;
        unordered_map<string, string> previous_map;
        priority_queue<node, vector<node>, greater<>> priority_queue;

        for (const auto& [airport, _] : flight_graph)
            distance_map[airport] = HUGE_VAL;
        distance_map[start] = 0;
        priority_queue.push(node(start, 0));

        while (!priority_queue.empty()) {
            auto [curr, curr_time] = priority_queue.top();
            priority_queue.pop();

            if (curr == end)
                return reconstruct_path(start, end, previous_map, distance_map[end]);

            if (curr_time > distance_map[curr]) continue;

            for (const auto& flight : flight_graph[curr]) {
                double new_dist = distance_map[curr] + flight.flight_time;
                if (new_dist < distance_map[flight.destination]) {
                    distance_map[flight.destination] = new_dist;
                    previous_map[flight.destination] = curr;
                    priority_queue.push(node(flight.destination, new_dist));
                }
            }
        }
        return { {"No route found", ""} };
    }
};

int main() {
    string filename = "T_T100_SEGMENT_ALL_CARRIER.csv";
    flight_router router(filename);

    while (true) {
        string start, end;
        cout << "\nStart airport (or 'exit' to quit): ";
        cin >> start;
        if (start == "exit") break;

        cout << "End airport: ";
        cin >> end;

        auto route = router.find_fastest_route(start, end);
        cout << "\nRoute:\n";
        for (size_t i = 0; i < route.size(); ++i) {
            cout << route[i].first << " (" << route[i].second << ")";
            if (i + 1 < route.size()) cout << " -> ";
            else cout << "\n";
        }
    }

    cout << "Program terminated.\n";
    return 0;
}