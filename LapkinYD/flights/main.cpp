#include "./preprocessor/preprocessor.h"
#include <fstream>
#include <iostream>
#include <map>
#include <cmath>

void find_shortest_path(const std::string& origin_code, const std::string& dest_code) {
    std::map<AirportId, std::map<AirportId, float>> flights;
    std::string csv_str;

    std::ifstream database_csv("database.csv");
    if (!database_csv.is_open()) {
        std::cout << "No database file\n";
        return;
    }

    while (std::getline(database_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        AirportId origin_id = std::stoi(elements[0]);
        AirportId dest_id = std::stoi(elements[1]);
        float air_time = std::stof(elements[2]);

        flights[origin_id][dest_id] = air_time;
    }
    database_csv.close();

    std::map<AirportCode, AirportId> airports;
    std::ifstream airports_csv("airports.csv");
    if (!airports_csv.is_open()) {
        std::cout << "No airports list\n";
        return;
    }

    while (std::getline(airports_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        AirportCode airport_code = elements[0];
        AirportId airport_id = std::stoi(elements[1]);

        airports[airport_code] = airport_id;
    }
    airports_csv.close();

    if (!airports.count(origin_code)) {
        std::cout << "No origin airport found\n";
        return;
    }
    AirportId origin_id = airports[origin_code];

    if (!airports.count(dest_code)) {
        std::cout << "No destination airport found\n";
        return;
    }
    AirportId dest_id = airports[dest_code];

    std::map<AirportId, Node> nodes;
    for (const auto& origin_list : flights) {
        nodes[origin_list.first] = {INFINITY, false, -1};
    }

    nodes[origin_id].min_time = 0;
    AirportId current_id = origin_id;

    while (current_id != -1) {
        for (const auto& flight : flights[current_id]) {
            float new_time = nodes[current_id].min_time + flight.second;
            if (new_time < nodes[flight.first].min_time) {
                nodes[flight.first].min_time = new_time;
                nodes[flight.first].prev_node = current_id;
            }
        }
        nodes[current_id].processed = true;

        current_id = -1;
        float min_time = INFINITY;
        for (const auto& node : nodes) {
            if (!std::isinf(node.second.min_time) && !node.second.processed && node.second.min_time < min_time) {
                min_time = node.second.min_time;
                current_id = node.first;
            }
        }
    }

    if (std::isinf(nodes[dest_id].min_time)) {
        std::cout << "No path\n";
    } else {
        std::cout << "Total flight time: " << nodes[dest_id].min_time << " minutes\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Wrong arguments\n";
        std::cout << "Usage: prog <origin code> <destination code>\n";
        return -1;
    }

    std::string origin_code(argv[1]);
    std::string dest_code(argv[2]);

    find_shortest_path(origin_code, dest_code);

    return 0;
}

