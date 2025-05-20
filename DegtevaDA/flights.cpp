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
    double air_time; // время в минутах
};

struct Airport {
    std::unordered_map<std::string, std::string> airport_names;

    void add_airport(const std::string& code, const std::string& name) {
        airport_names[code] = name;
    }

    std::string get_airport_name(const std::string& code) const {
        auto it = airport_names.find(code);
        return it != airport_names.end() ? it->second : code;
    }
};

class FlightGraph {
private:
    std::unordered_map<std::string, std::vector<Flight>> adj_list;
    Airport airport_data;

public:
    void add_flight(const std::string& origin, const std::string& destination, double air_time);
    std::pair<std::vector<std::string>, double> dijkstra(const std::string& start, const std::string& end);
    void add_airport(const std::string& code, const std::string& name) { airport_data.add_airport(code, name); }
    std::string get_airport_name(const std::string& code) const { return airport_data.get_airport_name(code); }
};

void FlightGraph::add_flight(const std::string& origin, const std::string& destination, double air_time) {
    adj_list[origin].push_back({destination, air_time});
}

std::pair<std::vector<std::string>, double> FlightGraph::dijkstra(const std::string& start, const std::string& end) {
    std::unordered_map<std::string, double> min_time;
    std::unordered_map<std::string, std::string> previous;
    std::priority_queue<std::pair<double, std::string>, std::vector<std::pair<double, std::string>>, std::greater<>> pq;

    // Инициализация всех вершин с бесконечностью
    for (const auto& [city, flights] : adj_list) {
        min_time[city] = std::numeric_limits<double>::infinity();
        for (const auto& flight : flights) {
            if (min_time.find(flight.destination) == min_time.end()) {
                min_time[flight.destination] = std::numeric_limits<double>::infinity();
            }
        }
    }

    if (min_time.find(start) == min_time.end() || min_time.find(end) == min_time.end()) {
        return {{}, -1}; // Ошибка: аэропорт не найден
    }

    min_time[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [current_time, current_city] = pq.top();
        pq.pop();

        if (current_city == end) break;

        for (const auto& flight : adj_list[current_city]) {
            double new_time = current_time + flight.air_time;
            if (new_time < min_time[flight.destination]) {
                min_time[flight.destination] = new_time;
                previous[flight.destination] = current_city;
                pq.push({new_time, flight.destination});
            }
        }
    }

    if (min_time[end] == std::numeric_limits<double>::infinity()) {
        return {{}, 0}; // Нет маршрута
    }

    std::vector<std::string> path;
    for (std::string at = end; !at.empty(); at = previous.count(at) ? previous[at] : "") {
        path.push_back(at);
        if (at == start) break;
    }
    std::reverse(path.begin(), path.end());
    return {path, min_time[end]};
}

void read_csv(const std::string& filename, FlightGraph& graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!\n";
        return;
    }

    std::string line;
    std::getline(file, line);
    std::stringstream header_stream(line);
    std::unordered_map<std::string, int> column_index;
    std::string column;
    int index = 0;

    while (std::getline(header_stream, column, ',')) {
        column_index[column] = index++;
    }

    int origin_idx = column_index["ORIGIN"];
    int dest_idx = column_index["DEST"];
    int air_time_idx = column_index["AIR_TIME"];
    int origin_city_idx = column_index["ORIGIN_CITY_NAME"];
    int dest_city_idx = column_index["DEST_CITY_NAME"];

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        if (row.size() > std::max({origin_idx, dest_idx, air_time_idx, origin_city_idx, dest_city_idx})) {
            std::string origin = row[origin_idx];
            std::string destination = row[dest_idx];
            double air_time = std::stod(row[air_time_idx]);
            std::string origin_city = row[origin_city_idx];
            std::string dest_city = row[dest_city_idx];

            if (air_time > 0) {
                graph.add_flight(origin, destination, air_time);
                graph.add_airport(origin, origin_city);
                graph.add_airport(destination, dest_city);
            }
        }
    }
}

void print_flight_path(const FlightGraph& graph, const std::string& start, const std::string& end, const std::vector<std::string>& path, double total_time) {
    if (total_time == -1) {
        std::cout << "Error: One or both airport codes not found.\n";
    } else if (path.empty()) {
        std::cout << "No available flight path from " << graph.get_airport_name(start)
                  << " to " << graph.get_airport_name(end) << ".\n";
    } else {
        std::cout << "The fastest route from " << graph.get_airport_name(start)
                  << " to " << graph.get_airport_name(end) << " takes "
                  << total_time << " minutes (~" << (int)(total_time/60) << "h " << (int)((int)total_time%60) << "m).\n";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << graph.get_airport_name(path[i]);
            if (i < path.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
    }
}

int main() {
    FlightGraph graph;
    std::string filename = "data.csv";
    read_csv(filename, graph);

    std::string start, end;
    std::cout << "Enter start airport code: ";
    std::cin >> start;
    std::cout << "Enter destination airport code: ";
    std::cin >> end;

    auto [path, total_time] = graph.dijkstra(start, end);
    print_flight_path(graph, start, end, path, total_time);

    return 0;
}
