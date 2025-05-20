#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>


struct Flight {
    std::string to;    
    double time;       
};


class AirportNames {
    std::unordered_map<std::string, std::string> names;

public:
    void add(const std::string& code, const std::string& name) {
        names[code] = name;
    }

    std::string get(const std::string& code) const {
        auto it = names.find(code);
        return (it != names.end()) ? it->second : code;
    }
};


class Flights {
    std::unordered_map<std::string, std::vector<Flight>> routes;
    AirportNames airport_names;

public:
    void add_flight(const std::string& from, const std::string& to, double time) {
        routes[from].push_back({to, time});
    }

    void add_airport_name(const std::string& code, const std::string& name) {
        airport_names.add(code, name);
    }

    std::string get_airport_name(const std::string& code) const {
        return airport_names.get(code);
    }

    void find_fastest_path(const std::string& start, const std::string& end) {
        std::unordered_map<std::string, double> min_time;
        std::unordered_map<std::string, std::string> previous;
        using QueueItem = std::pair<double, std::string>;

        std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> queue;

        for (const auto& [city, _] : routes) {
            min_time[city] = std::numeric_limits<double>::infinity();
        }

        min_time[start] = 0;
        queue.push({0, start});

        while (!queue.empty()) {
            auto [current_time, current_city] = queue.top();
            queue.pop();

            if (current_city == end) break;

            for (const auto& flight : routes[current_city]) {
                double total_time = current_time + flight.time;
                if (total_time < min_time[flight.to]) {
                    min_time[flight.to] = total_time;
                    previous[flight.to] = current_city;
                    queue.push({total_time, flight.to});
                }
            }
        }

        if (min_time[end] == std::numeric_limits<double>::infinity()) {
            std::cout << "No available route from " << get_airport_name(start)
                      << " to " << get_airport_name(end) << "\n";
            return;
        }

        std::vector<std::string> path;
        for (std::string city = end; !city.empty(); city = previous[city]) {
            path.push_back(city);
            if (previous.find(city) == previous.end()) break;
        }

        std::reverse(path.begin(), path.end());

        std::cout << "\nFastest route found (" << min_time[end] << " minutes):\n";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << get_airport_name(path[i]);
            if (i < path.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
    }
};


void load_data(const std::string& filename, Flights& graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << "\n";
        return;
    }

    std::string line;
    std::getline(file, line); 

    std::unordered_map<std::string, int> columns;
    std::stringstream ss(line);
    std::string column;
    int index = 0;
    while (std::getline(ss, column, ',')) {
        columns[column] = index++;
    }

    while (std::getline(file, line)) {
        std::stringstream row_stream(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(row_stream, cell, ',')) {
            row.push_back(cell);
        }

        std::string from = row[columns["ORIGIN"]];
        std::string to = row[columns["DEST"]];
        std::string from_name = row[columns["ORIGIN_CITY_NAME"]];
        std::string to_name = row[columns["DEST_CITY_NAME"]];
        double time = std::stod(row[columns["AIR_TIME"]]);

        if (time > 0) {
            graph.add_flight(from, to, time);
            graph.add_airport_name(from, from_name);
            graph.add_airport_name(to, to_name);
        }
    }
}


int main() {
    Flights graph;

    std::string filename = "C:/c/data.csv";
    load_data(filename, graph);

    std::string from, to;
    std::cout << "\nEnter the departure airport code: ";
    std::cin >> from;
    std::cout << "Enter the destination airport code: ";
    std::cin >> to;

    graph.find_fastest_path(from, to);
}
