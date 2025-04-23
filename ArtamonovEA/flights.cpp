#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

using namespace std;


struct Flight {
    string dest;
    double time;
};


using Graph = unordered_map<string, vector<Flight>>;
using AirportInfo = unordered_map<string, string>;
using DistMap = unordered_map<string, double>;
using PrevMap = unordered_map<string, string>;


struct Node {
    string airport;
    double time;
    bool operator>(const Node& n) const { return time > n.time; }
};


class FlightRouter {
    Graph graph;
    AirportInfo airports;

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
                } else current_value += c;
            }
            if (!current_value.empty()) row.push_back(current_value);

            if (row.size() > max({origin_idx, dest_idx, air_time_idx, 
                                origin_city_idx, dest_city_idx})) {
                string origin = row[origin_idx];
                string dest = row[dest_idx];
                double air_time = stod(row[air_time_idx]);
                string origin_city = row[origin_city_idx];
                string dest_city = row[dest_city_idx];

                if (air_time > 0) {
                    graph[origin].push_back({dest, air_time});
                    airports[origin] = origin_city;
                    airports[dest] = dest_city;
                }
            }
        }
    }

    vector<pair<string, string>> reconstructPath(const string& start, const string& end,
                                               const PrevMap& prev, double totalTime) {
        vector<pair<string, string>> path;
        string curr = end;

        while (!curr.empty()) {
            path.emplace_back(curr, airports[curr]);
            if (curr == start) break;
            curr = prev.at(curr);
        }

        if (path.back().first != start) return {{"No route found", ""}};
        
        reverse(path.begin(), path.end());
        cout << "Total time: " << totalTime << " min (" 
             << totalTime/60 << " hr)\n";
        return path;
    }

public:
    FlightRouter(const string& filename) { load_from_csv(filename); }

    vector<pair<string, string>> findFastestRoute(string start, string end) {
        if (!graph.count(start) || !graph.count(end)) 
            return {{"Airport not found", ""}};

        DistMap dist;
        PrevMap prev;
        priority_queue<Node, vector<Node>, greater<>> pq;

        for (const auto& [airport, _] : graph) 
            dist[airport] = HUGE_VAL;
        dist[start] = 0;
        pq.push({start, 0});

        while (!pq.empty()) {
            auto [curr, currTime] = pq.top();
            pq.pop();

            if (curr == end) 
                return reconstructPath(start, end, prev, dist[end]);
            
            if (currTime > dist[curr]) continue;

            for (const auto& flight : graph[curr]) {
                double newDist = dist[curr] + flight.time;
                if (newDist < dist[flight.dest]) {
                    dist[flight.dest] = newDist;
                    prev[flight.dest] = curr;
                    pq.push({flight.dest, newDist});
                }
            }
        }
        return {{"No route found", ""}};
    }
};


int main() {
    std::string filename = "/Users/egor/PycharmProjects/MR2024_broken/0/graph/T_T100_SEGMENT_ALL_CARRIER_20240316_162248/T_T100_SEGMENT_ALL_CARRIER.csv";

    FlightRouter router(filename);

    string start, end;
    cout << "Start airport: "; cin >> start;
    cout << "End airport: "; cin >> end;

    auto route = router.findFastestRoute(start, end);
    cout << "\nRoute:\n";
    for (size_t i = 0; i < route.size(); ++i) {
        cout << route[i].first << " (" << route[i].second << ")";
        if (i + 1 < route.size()) cout << " -> ";
        else cout << "\n";
    }

    return 0;
}