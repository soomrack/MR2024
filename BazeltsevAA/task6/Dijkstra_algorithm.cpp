#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <limits>

struct edge {
    std::string destination;
    int air_time; // время в минутах
};

struct airport_info {
    std::string city_name; 
    std::vector<edge> edges;
};


std::vector<std::string> separation_dijkstra(const std::string &line, const char delimiter_sign, const char exclusion_sign) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_exclusion = false;
    
    for (int index = 0; index < line.size(); index++) {
        if (line[index] == exclusion_sign) {
            in_exclusion = !in_exclusion;
        }
        else if (line[index] == delimiter_sign && (in_exclusion == false)) {
            tokens.push_back(token);
            token.clear();  
        }
        else {
            token += line[index];
        }
    }

    tokens.push_back(token);
    return tokens;
}


std::unordered_map<std::string, airport_info> airport_data() {
    std::unordered_map<std::string, airport_info> airports;
    
    std::ifstream in_file;
    in_file.open("airoport_sorting.csv");

    if (in_file.is_open() == false) {
        std::cerr << "Ошибка: не удалось открыть файл" << std::endl;
        return {};
    }

    std::string line;
    while (std::getline(in_file, line)) {
        std::vector<std::string> cell = separation_dijkstra(line, ',', '"');

        if (cell.size() >= 6) {
            std::string origin = cell[2];
            std::string origin_city = cell[3];
            std::string dest = cell[4];
            std::string dest_city = cell[5];

            int air_time = 0;
            try {
                air_time = stoi(cell[1]);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Время перелёта не является числом: " << cell[1] << std::endl;
            }

            if (airports.find(origin) == airports.end()) {
                airports[origin] = {origin_city, {}}; 
            }

            airports[origin].edges.push_back({dest, air_time});

            if (airports.find(dest) == airports.end()) {
                airports[dest] = {dest_city, {}};
            }
        }
    }

    in_file.close();
    return airports;
}


std::pair<std::vector<std::string>, int> find_shortest_path(
    const std::unordered_map<std::string, airport_info>& airports, 
    const std::string& start, 
    const std::string& end) {

    if (airports.find(start) == airports.end() || airports.find(end) == airports.end()) {
        return {{}, -1};
    }

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;
    std::unordered_map<std::string, bool> visited;

    for (const auto& entry : airports) {
        dist[entry.first] = std::numeric_limits<int>::max();
        visited[entry.first] = false;
    }

    dist[start] = 0;

    while (true) {
        std::string current;
        int min_dist = std::numeric_limits<int>::max();
        
        for (const auto& entry : dist) {
            if (!visited[entry.first] && entry.second < min_dist) {
                min_dist = entry.second;
                current = entry.first;
            }
        }

        if (current.empty() || min_dist == std::numeric_limits<int>::max()) {
            break;
        }

        if (current == end) {
            break;
        }

        visited[current] = true;

        for (const edge& e : airports.at(current).edges) {
            std::string neighbor = e.destination;
            if (visited[neighbor]) continue;

            int new_dist = dist[current] + e.air_time;
            if (new_dist < dist[neighbor]) {
                dist[neighbor] = new_dist;
                prev[neighbor] = current;
            }
        }
    }

    if (dist[end] == std::numeric_limits<int>::max()) {
        return {{}, -1};
    }

    std::vector<std::string> path;
    for (std::string at = end; at != ""; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {path, dist[end]};
}


void flight_task() {
    auto airports = airport_data();

    std::string start_airport, end_airport;
    std::cout << "Введите код аэропорта вылета: ";
    std::cin >> start_airport;
    std::cout << "Введите код аэропорта прибытия: ";
    std::cin >> end_airport;

    transform(start_airport.begin(), start_airport.end(), start_airport.begin(), ::toupper);
    transform(end_airport.begin(), end_airport.end(), end_airport.begin(), ::toupper);

    auto result = find_shortest_path(airports, start_airport, end_airport);
    auto& path = result.first;
    int summ_time = result.second;

    if (path.empty() || summ_time == -1) {
        std::cout << "Маршрут между " << start_airport << " и " << end_airport << " не найден" << std::endl;
    } else {
        std::cout << "\nЛучший маршрут маршрут:" << std::endl;
        for (size_t i = 0; i < path.size(); ++i) {
            if (i != 0) std::cout << " -> ";
            std::cout << airports.at(path[i]).city_name;
        }
        std::cout << "\nОбщее время в пути:\n" << summ_time << " минут" << std::endl;
    }
}
