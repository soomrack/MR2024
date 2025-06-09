#include <fstream>
#include <iostream>
#include <chrono>

#include "pathfinder.h"

std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> splitted_line;
    std::string word;
    bool in_quotes = false;

    for (char letter : line) {
        if (letter == '"') {
            in_quotes = !in_quotes;
        }
        else if (letter == ',' && !in_quotes) {
            splitted_line.push_back(word);
            word.clear();
        }
        else {
            word += letter;
        }
    }
    splitted_line.push_back(word);

    return splitted_line;
}

// получение данных из CSV и запись в граф
std::unordered_map<std::string, Point> get_airport_data(const std::string& file_name)
{
    std::ifstream file(file_name);

    // Порядковые номера нужных позиций из CSV
    size_t distance_file_number = 7;
    size_t air_time_file_number = 9;
    size_t origin_file_number = 22;
    size_t origin_city_name_file_number = 23;
    size_t destination_file_number = 33;
    size_t destination_city_name_file_number = 34;
    
    size_t year_file_number = 44;
    size_t month_file_number = 46;

    std::unordered_map<std::string, Point> airports;

    if (!file.is_open()) {
        std::cerr << "File was not opened";
        throw std::invalid_argument("File was not opened");
    }

    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> words = split(line);

        int air_time = 0;
        int distance = 0;
        if (words[distance_file_number] != "DISTANCE" && stoi(words[1]) == 29) {
            try {
                if (stoi(words[air_time_file_number]) == 0) {
                    air_time = std::numeric_limits<int>::max();
                }
                else {
                    air_time = stoi(words[air_time_file_number]);
                }
                distance = stoi(words[distance_file_number]);
            }
            catch (...) {
                std::cerr << "Invalid air time: " << words[air_time_file_number] << "or ";
                std::cerr << "Invalid distance: " << words[distance_file_number];
                continue;
            }
            std::string origin = words[origin_file_number];
            std::string origin_city_name = words[origin_city_name_file_number];
            std::string destination = words[destination_file_number];
            std::string destination_city_name = words[destination_city_name_file_number];

            // Добавление аэропорта отправления
            if (airports.find(origin) == airports.end()) {
                airports[origin] = { origin_city_name, {} };
            }

            // Добавление аэропорта прибытия
            if (airports.find(destination) == airports.end()) {
                airports[destination] = { destination_city_name, {} };
            }

            // Добавление в граф
            airports[origin].edges.push_back({destination, air_time});
        }
    }

    file.close();
    return airports;
}

std::tuple<std::unordered_map<std::string, Point>, std::string, std::string> request(const std::string& file_name) {
    auto airports = get_airport_data(file_name);

    std::string start_airport;
    std::string finish_airport;
    std::cout << "Enter start airport code: ";
    std::cin >> start_airport;
    std::cout << "Enter finish airport code: ";
    std::cin >> finish_airport;
    
    return {airports, start_airport, finish_airport};
}

void print_result(const std::string& file_name, const std::string& algoritm_name) {
    auto req = request(file_name);
    std::unordered_map<std::string, Point> airports = std::get<0>(req);
    std::string start_airport = std::get<1>(req);
    std::string finish_airport = std::get<2>(req);
    
    auto begin = std::chrono::steady_clock::now();
    
    std::vector<std::string> path;
    int total_time;
    if (algoritm_name == "Bellman-Ford") {
        path = BellmanFord(airports, start_airport, finish_airport).first;
        total_time = BellmanFord(airports, start_airport, finish_airport).second;
    }
    else if (algoritm_name == "Floyd-Warshall") {
        path = FloydWarshall(airports, start_airport, finish_airport).first;
        total_time = FloydWarshall(airports, start_airport, finish_airport).second;
    }
    else {
        std::cout << "Incorrect algoritm name" << std::endl;
        return;
    }
    if (path.empty() || total_time == -1) {
        std::cout << "No path found" << std::endl;
    }
    else {
        std::cout << "Optimal path:" << std::endl;
        for (size_t idx = 0; idx < path.size(); idx++) {
            if (idx != 0) std::cout << " - ";
            std::cout << airports[path[idx]].point;
        }
        std::cout << "\ntotal time: " << total_time << " minutes" << std::endl;
    }
    
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "  algoritm time: " << time.count() << " ms\n";
}

int main() {
    std::cout << "Bellman-Ford:" << std::endl;
    print_result("T_T100_SEGMENT_ALL_CARRIER.csv", "Bellman-Ford");
    
    std::cout << "Floyd-Warshall:" << std::endl;
    print_result("T_T100_SEGMENT_ALL_CARRIER.csv", "Floyd-Warshall");
    
    return 1;
}