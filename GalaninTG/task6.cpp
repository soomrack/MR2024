#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>


std::vector<std::string> split(const std::string &line, char separator) {
    std::vector<std::string> sublines;
    std::string subline;
    bool quotes = false;

    for (char c : line) {
        if (c == '"') { quotes = !quotes; }
        else if (c == separator && !quotes) {
            sublines.push_back(subline);
            subline.clear();
        } else {
            subline += c;
        }
    }
    sublines.push_back(subline);
    return sublines;
}


std::string first_word(const std::string str) {
    size_t comma = str.find(',');
    if (comma != std::string::npos) {
        return str.substr(0, comma);
    }
    return str;
}


void make_new_file(const std::string& file_path) {
    std::ifstream input_file(file_path);
    std::ofstream output_file("recive.csv");

    if (!input_file.is_open()) {
        std::cerr << "Ошибка открытия исходного файла" << std::endl;
        return;
    }
    
    if (!output_file.is_open()) {
        std::cerr << "Ошибка создания выходного файла" << std::endl;
        return;
    }

    std::string headers_line;
    std::getline(input_file, headers_line);
    std::vector<std::string> headers = split(headers_line,',');    

    struct ColumnIndices {
        int origin = -1;
        int dest = -1;
        int air_time = -1;
        int month = -1;
    } col_idx;

    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == "ORIGIN_CITY_NAME") col_idx.origin = i;
        else if (headers[i] == "DEST_CITY_NAME") col_idx.dest = i;
        else if (headers[i] == "AIR_TIME") col_idx.air_time = i;
        else if (headers[i] == "MONTH") col_idx.month = i;
    }

    if (col_idx.origin == -1 || col_idx.dest == -1 || 
        col_idx.air_time == -1 || col_idx.month == -1) {
        std::cerr << "Ошибка: В заголовке отсутствуют необходимые колонки" << std::endl;
    }

    std::string line;

    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        
        if (cells.size() >= col_idx.month) {
            std::string origin_city = first_word(cells[col_idx.origin]);
            std::string dest_city = first_word(cells[col_idx.dest]);
            
            output_file << origin_city << "," 
                        << dest_city << "," 
                        << col_idx.air_time << std::endl;
        } else {
            std::cerr << "Ошибка: строка не содержит достаточно жлементов" << std::endl;
        }
    }
    
    input_file.close();
    output_file.close();
}


bool air_time_control(const std::string& air_time_str) {
    try {
        float air_time = std::stof(air_time_str);
        return air_time != 0.0f;
    } catch (const std::invalid_argument& e) { 
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}


void filter_file(unsigned int periodicity) {
    std::map<std::pair<std::string, std::string>, int> flight_count;

    std::ifstream input_file("recive.csv");

    if (!input_file.is_open()) {
        std::cerr << "Ошибка открытия исходного файла" << std::endl;
        return;
    }
    
    std::string line;

    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (air_time_control(cells[3])) {
            std::string origin = cells[1];
            std::string dest = cells[2];
            flight_count[{origin, dest}]++;
        }
    }
    input_file.close();
    
    input_file.open("recive.csv");
    std::ofstream output_file("output.csv");
        
    if (!input_file.is_open()) {
        std::cerr << "Ошибка открытия исходного файла" << std::endl;
        return;
    }
    
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (air_time_control(cells[3])) {
            std::string origin = cells[1];
            std::string dest = cells[2];
            if (flight_count[{origin, dest}] >= periodicity) {
                output_file << line << std::endl;
            }
        }
    }
    
    input_file.close();
    output_file.close();
    remove("recive.csv");
}


struct Route {
    std::string destination;
    int air_time;
};


std::unordered_map<std::string, std::vector<Route>> graph() {

    std::unordered_map<std::string, std::vector<Route>> new_graph;
    
    std::ifstream input_file("output.csv");
    if (!input_file.is_open()) {
        std::cerr << "Ошибка открытия исходного файла" << std::endl;
        return;
    };

    std::string line;

    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        new_graph[cells[1]].push_back({cells[2], std::stoi(cells[3])});
        new_graph[cells[2]]; 
    }

    return new_graph;
}


std::pair<std::vector<std::string>, int> shortest_path (const std::string origin, const std::string dest, 
const std::unordered_map<std::string, std::vector<Route>> graph) {

    if (graph.find(origin) == graph.end() || graph.find(dest) == graph.end()) {
        return {{}, -1};
    }
    
    std::priority_queue<std::pair<int, std::string>, 
                        std::vector<std::pair<int, std::string>>, 
                        std::greater<std::pair<int, std::string>>> pq;

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;

    for (const auto& entry : graph) {
        dist[entry.first] = std::numeric_limits<int>::max();
    }

    dist[origin] = 0;
    pq.push({0, origin});

    while (!pq.empty()) {
        std::string current = pq.top().second;
        int current_dist = pq.top().first;
        pq.pop();

        if (current == dest) break;
        if (current_dist > dist[current]) continue;

        for (const Route& route : graph.at(current)) {
            std::string neighbor = route.destination;
            int new_dist = dist[current] + route.air_time;

            if (new_dist < dist[neighbor]) {
                dist[neighbor] = new_dist;
                prev[neighbor] = current;
                pq.push({new_dist, neighbor});
            }
        }
    }

    if (dist[dest] == std::numeric_limits<int>::max()) {
        return {{}, -1};
    }

    std::vector<std::string> path;
    for (std::string at = dest; at != ""; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {path, dist[dest]};
}


void print_shortest_path(const std::string& origin, 
    const std::string& dest) {

    std::unordered_map<std::string, std::vector<Route>> flight_graph = graph();


    auto result = shortest_path(origin, dest, flight_graph);
    std::vector<std::string> path = result.first;
    int total_time = result.second;

    if (path.empty() || total_time == -1) {
        std::cout << "Маршрут между " << origin << " и " << dest << " не найден." << std::endl;
        return;
    }

    std::cout << "Кратчайший маршрут:" << std::endl;

    for (size_t i = 0; i < path.size(); ++i) {

        if (i != 0) std::cout << " -> ";

        std::cout << path[i];
    }

std::cout << std::endl << "Общее время в пути: " << total_time << " минут" << std::endl;
}


int main() {

    make_new_file("T_T100_SEGMENT_ALL_CARRIER.csv");

    filter_file(100);
    
    print_shortest_path("New York", "Los Angeles");
    
    return 0;
}
