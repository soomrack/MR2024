#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>


struct Edge {
    std::string destination;
    int air_time;
};


struct AirportInfo {
    std::string city_name;
    std::vector<Edge> edges;
};


std::vector<std::string> split_d(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    
    for (char c : line) {
        if (c == ',') {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    
    return tokens;
}


// Загрузка данных из CSV
std::unordered_map<std::string, AirportInfo> load_airport_data(const std::string& filename) {
    std::unordered_map<std::string, AirportInfo> airports;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        throw std::invalid_argument("No such a file!");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> cells = split_d(line);

        if (cells.size() >= 6) {
            std::string origin = cells[2];
            std::string origin_city = cells[3];
            std::string dest = cells[4];
            std::string dest_city = cells[5];
            
            int air_time = 0;
            try {
                air_time = stoi(cells[1]);
            } catch (...) {
                std::cerr << "Invalid air time value: " << cells[1] << std::endl;
                continue;
            }

            // Добавление аэропорт отправления, если его нет
            if (airports.find(origin) == airports.end()) {
                airports[origin] = {origin_city, {}};
            }

            // Добавление ребро в граф
            airports[origin].edges.push_back({dest, air_time});

            // Добавление аэропорт прибытия, если его нет
            if (airports.find(dest) == airports.end()) {
                airports[dest] = {dest_city, {}};
            }
        }
    }

    file.close();
    return airports;
}


// Реализация алгоритма Дейкстры
std::pair<std::vector<std::string>, int> find_shortest_path
(const std::unordered_map<std::string, AirportInfo>& airports, 
const std::string& start, const std::string& end) {

    if (airports.find(start) == airports.end() || airports.find(end) == airports.end()) {
        return {{}, -1};
    }

    // Инициализация
    std::priority_queue<std::pair<int, std::string>, 
                        std::vector<std::pair<int, std::string>>, 
                        std::greater<std::pair<int, std::string>>> pq;
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;

    for (const auto& entry : airports) {
        dist[entry.first] = std::numeric_limits<int>::max();
    }

    dist[start] = 0;
    pq.push({0, start});

    // Основной цикл
    while (!pq.empty()) {
        std::string current = pq.top().second;
        int current_dist = pq.top().first;
        pq.pop();

        if (current == end) break;
        if (current_dist > dist[current]) continue;

        for (const Edge& edge : airports.at(current).edges) {
            std::string neighbor = edge.destination;
            int new_dist = dist[current] + edge.air_time;

            if (new_dist < dist[neighbor]) {
                dist[neighbor] = new_dist;
                prev[neighbor] = current;
                pq.push({new_dist, neighbor});
            }
        }
    }

    if (dist[end] == std::numeric_limits<int>::max()) {
        return {{}, -1};
    }

    // Восстановление пути
    std::vector<std::string> path;
    for (std::string at = end; at != ""; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {path, dist[end]};
}


void user_request() {
    auto airports = load_airport_data("output.csv");

    std::string start_airport, end_airport;
    std::cout << "Enter departure airport code: ";
    std::cin >> start_airport;
    std::cout << "Enter arrival airport code: ";
    std::cin >> end_airport;

    transform(start_airport.begin(), start_airport.end(), start_airport.begin(), ::toupper);
    transform(end_airport.begin(), end_airport.end(), end_airport.begin(), ::toupper);

    auto result = find_shortest_path(airports, start_airport, end_airport);
    auto& path = result.first;
    int total_time = result.second;

    if (path.empty() || total_time == -1) {
        std::cout << "No route found between " << start_airport << " and " << end_airport << std::endl;
    } else {
        std::cout << "\nOptimal route:" << std::endl;
        for (size_t i = 0; i < path.size(); ++i) {
            if (i != 0) std::cout << " -> ";
            std::cout << airports[path[i]].city_name;
        }
        std::cout << "\nTotal time:\n" << total_time << " minutes" << std::endl;
    }
}
