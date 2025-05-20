#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>

struct Flight {
    std::string destination;
    double air_time;
};

struct AirportData {
    std::unordered_map<std::string, std::string> code_to_city;

    void add(const std::string& code, const std::string& city_name) {
        code_to_city[code] = city_name;
    }

    std::string get(const std::string& code) const {
        auto it = code_to_city.find(code);
        return (it != code_to_city.end()) ? it->second : code;
    }
};

class FlightGraph {
private:
    std::unordered_map<std::string, std::vector<Flight>> adj;
    AirportData airport_names;

public:
    void add_flight(const std::string& origin, const std::string& dest, double air_time) {
        adj[origin].push_back({dest, air_time});
    }

    void add_airport(const std::string& code, const std::string& name) {
        airport_names.add(code, name);
    }

    std::pair<std::vector<std::string>, double> find_shortest_route(const std::string& start, const std::string& end) {
        using QueueNode = std::pair<double, std::string>; // {total_time, airport}
        std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<>> pq;

        std::unordered_map<std::string, double> time;
        std::unordered_map<std::string, std::string> prev;

        for (const auto& [airport, _] : adj) {
            time[airport] = std::numeric_limits<double>::infinity();
        }

        if (time.find(start) == time.end()) {
            return {{}, 0}; // Стартового аэропорта нет в графе
        }

        time[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [curr_time, curr_airport] = pq.top();
            pq.pop();

            if (curr_airport == end) break;

            for (const auto& flight : adj[curr_airport]) {
                double new_time = curr_time + flight.air_time;
                if (new_time < time[flight.destination]) {
                    time[flight.destination] = new_time;
                    prev[flight.destination] = curr_airport;
                    pq.push({new_time, flight.destination});
                }
            }
        }

        if (time[end] == std::numeric_limits<double>::infinity()) {
            return {{}, 0}; // путь не найден
        }

        // Восстановление пути
        std::vector<std::string> path;
        std::string at = end;
        while (at != start) {
            path.push_back(at);
            if (prev.find(at) == prev.end()) {
                return {{}, 0};  // некорректный путь
            }
            at = prev[at];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());

        return {path, time[end]};
    }

    void print_route(const std::vector<std::string>& path, double total_time) const {
        if (path.empty()) {
            std::cout << "No route found.\n";
            return;
        }

        std::cout << "Shortest route (" << total_time << " minutes):\n";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i] << " (" << airport_names.get(path[i]) << ")";
            if (i + 1 < path.size()) std::cout << " -> ";
        }
        std::cout << '\n';
    }

    bool has_airport(const std::string& code) const {
        return adj.find(code) != adj.end();
    }

    friend void read_csv(const std::string& filename, FlightGraph& graph);
        void list_airports() const {
        std::cout << "\nAvailable airports:\n";
        for (const auto& [code, _] : adj) {
            std::cout << "  " << code << " (" << airport_names.get(code) << ")\n";
        }
        std::cout << "Total: " << adj.size() << " airports.\n\n";
    }

};

void read_csv(const std::string& filename, FlightGraph& graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Open file error: " << filename << "\n";
        return;
    }

    std::string line;
    std::getline(file, line); // заголовок

    std::unordered_map<std::string, int> column_index;
    std::stringstream header_ss(line);
    std::string column;
    int col_idx = 0;
    while (std::getline(header_ss, column, ',')) {
        column_index[column] = col_idx++;
    }

    // Проверка наличия нужных колонок
    if (column_index.count("ORIGIN") == 0 || column_index.count("DEST") == 0 ||
        column_index.count("AIR_TIME") == 0 || column_index.count("ORIGIN_CITY_NAME") == 0 ||
        column_index.count("DEST_CITY_NAME") == 0) {
        std::cerr << "The required columns were not found in the CSV file.\n";
        return;
    }

    int idx_origin = column_index["ORIGIN"];
    int idx_dest = column_index["DEST"];
    int idx_air_time = column_index["AIR_TIME"];
    int idx_origin_city = column_index["ORIGIN_CITY_NAME"];
    int idx_dest_city = column_index["DEST_CITY_NAME"];

    int line_number = 1;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        if (row.size() <= std::max({idx_origin, idx_dest, idx_air_time, idx_origin_city, idx_dest_city}))
            continue;  // строка неполная

        try {
            std::string origin = row[idx_origin];
            std::string dest = row[idx_dest];
            std::string origin_city = row[idx_origin_city];
            std::string dest_city = row[idx_dest_city];

            if (row[idx_air_time].empty()) continue;
            double air_time = std::stod(row[idx_air_time]);

            if (air_time > 0) {
                graph.add_flight(origin, dest, air_time);
                graph.add_airport(origin, origin_city);
                graph.add_airport(dest, dest_city);
            }
        } catch (...) {
            std::cerr << "Warning: Skipping malformed line " << line_number << "\n";
        }

        ++line_number;
    }
}


int main() {
    FlightGraph graph;
    std::string filename = "C:/C/T_T100_SEGMENT_ALL_CARRIER.csv";
    read_csv(filename, graph);

    graph.list_airports();  // Показать все доступные аэропорты

    std::string start, end;
    std::cout << "Enter start airport code: ";
    std::cin >> start;
    std::cout << "Enter destination airport code: ";
    std::cin >> end;

    if (start == end) {
        std::cout << "Start and destination are the same.\n";
        return 0;
    }

    if (!graph.has_airport(start)) {
        std::cerr << "Start airport '" << start << "' not found in data.\n";
        return 1;
    }

    if (!graph.has_airport(end)) {
        std::cerr << "Destination airport '" << end << "' not found in data.\n";
        return 1;
    }

    auto [path, total_time] = graph.find_shortest_route(start, end);
    graph.print_route(path, total_time);

    return 0;
}
