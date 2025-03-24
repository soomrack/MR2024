#include "airlines_list.hpp"
#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <queue>


GL::AirlinesList& GL::AirlinesList::get_instance() noexcept {
    static AirlinesList instance;
    return instance;
}


std::vector<std::string> GL::AirlinesList::parse_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string curr_cell;
    bool inside_quotes = false;
    char prev_char = '\0';

    for (char curr_char : line) {
        if (curr_char == '"' && prev_char != '\\') {
            inside_quotes = !inside_quotes;
        } else if (curr_char == ',' && !inside_quotes) {
            cells.push_back(curr_cell);
            curr_cell.clear();
        } else {
            curr_cell += curr_char;
        }
        prev_char = curr_char;
    }

    if (!curr_cell.empty()) {
        cells.push_back(curr_cell);
    }

    return cells;
}


void GL::AirlinesList::load_data(const std::string& csv_file) {
    if (!graph.empty()) this->clear();

    std::ifstream file(csv_file);
    if (!file.is_open()) {
        throw AirlinesListException("Failed to open file: " + csv_file);
    }

    std::string line;

    if (!std::getline(file, line)) {
        throw AirlinesListException("Empty file or missing header");
    }

    std::unordered_map<std::string, size_t> column_indexes;
    std::vector<std::string> headers = parse_csv_line(line);
    for (size_t i = 0; i < headers.size(); ++i) {
        column_indexes[headers[i]] = i;
    }


    size_t row_number = 0;
    while (std::getline(file, line)) {
        row_number++;
        std::vector<std::string> row = parse_csv_line(line);

        if (row.size() < column_indexes.size()) {
            throw AirlinesListException("Invalid row in CSV file at line " + std::to_string(row_number));
        }

        FlightInfo info;

        try {
            info.air_time = std::stod(row[column_indexes.at("AIR_TIME")]);
            info.origin_airport_id = std::stoi(row[column_indexes.at("ORIGIN_AIRPORT_ID")]);
            info.dest_airport_id = std::stoi(row[column_indexes.at("DEST_AIRPORT_ID")]);
            info.year = std::stoi(row[column_indexes.at("YEAR")]);
            info.quarter = std::stoi(row[column_indexes.at("QUARTER")]);
            info.month = std::stoi(row[column_indexes.at("MONTH")]);
        } catch (const std::invalid_argument& e) {
            throw AirlinesListException("Error parsing data at line " + std::to_string(row_number) + ": " + e.what());
        }

        airports.insert(info.origin_airport_id);
        airports.insert(info.dest_airport_id);

        graph[info.origin_airport_id].emplace_back(info.dest_airport_id, info);
    }

    file.close();
}


void GL::AirlinesList::clear() noexcept {
    graph.clear();
    airports.clear();
}


bool GL::AirlinesList::contains_airport(const AirportID index) const noexcept {
    return (airports.find(index) != airports.end()); 
}


GL::FlightPath GL::AirlinesList::find_flight_path_between(const AirportID origin_index, const AirportID dest_index) const {
    if(!this->contains_airport(origin_index)) {
        throw AirlinesListException("Origin airport index not found: " + std::to_string(origin_index));
    }

    if(!this->contains_airport(dest_index)) {
        throw AirlinesListException("Dest airport index not found: " + std::to_string(dest_index));
    }

    FlightPathRow path = dijkstra(origin_index);

    return reconstruct_path(path, dest_index);
}


GL::AirlinesList::FlightPathRow GL::AirlinesList::dijkstra(const AirportID origin_index) const noexcept {
    std::unordered_map<int, double> min_distance;
    std::unordered_map<int, int> parents_airports;

    for (int airport : airports) {
        min_distance[airport] = std::numeric_limits<double>::max();
        parents_airports[airport] = -1;
    }

    min_distance[origin_index] = 0;

    using Pair = std::pair<double, int>; // {air_time, airport index}
    auto cmp = [](const Pair& a, const Pair& b) { return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> min_distance_queue(cmp);

    min_distance_queue.push({0, origin_index});

    while (!min_distance_queue.empty()) {
        auto [curr_distance, curr_dest] = min_distance_queue.top();
        min_distance_queue.pop();

        if (curr_distance > min_distance[curr_dest]) continue;
        if (graph.find(curr_dest) == graph.end()) continue;

        for (const auto& [dest_vertex, edge] : graph.at(curr_dest)) {
            if (min_distance[dest_vertex] > min_distance[curr_dest] + edge.air_time) {
                min_distance[dest_vertex] = min_distance[curr_dest] + edge.air_time;
                parents_airports[dest_vertex] = curr_dest;
                min_distance_queue.push({min_distance[dest_vertex], dest_vertex});
            }
        }
    }

    return {min_distance, parents_airports};
}


GL::FlightPath GL::AirlinesList::reconstruct_path(GL::AirlinesList::FlightPathRow& path, const AirportID dest_index) const noexcept {
    std::vector<int> reconstruct_parents;
    std::vector<double> reconstruct_distances;

    for(int curr_index = dest_index; curr_index != -1; curr_index = path.second[curr_index]) {
        reconstruct_parents.push_back(curr_index);
    }

    std::reverse(reconstruct_parents.begin(), reconstruct_parents.end());

    for(size_t i = 1; i < reconstruct_parents.size(); ++i) {
        int prev_index = reconstruct_parents[i - 1];
        int curr_index = reconstruct_parents[i];

        if (graph.find(prev_index) == graph.end()) continue;

        for (const auto& [dest_vertex, edge] : graph.at(prev_index)) {
            if (dest_vertex == curr_index) {
                reconstruct_distances.push_back(edge.air_time);
                break;
            }
        }
    }

    return {reconstruct_parents, reconstruct_distances};
}

