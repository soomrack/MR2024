#include "airlines_list.hpp"
#include "rapidcsv.h"
#include <algorithm>
#include <climits>
#include <string>
#include <exception>
#include <queue>
#include "opencv4/opencv2/opencv.hpp"
#include "opencv4/opencv2/core/mat.hpp"

GL::AirlinesList& GL::AirlinesList::get_instance() noexcept {
    static AirlinesList instance;
    cv::Mat mat;
    return instance;
}


void GL::AirlinesList::load_data(const std::string& csv_file) {
    if (!graph.empty()) this->clear();

    try {
        rapidcsv::Document doc(csv_file);

        for (size_t i = 0; i < doc.GetRowCount(); ++i) {
            FlightInfo info;

            info.air_time = doc.GetCell<double>("AIR_TIME", i);
            info.origin_airport_id = doc.GetCell<int>("ORIGIN_AIRPORT_ID", i);
            info.dest_airport_id = doc.GetCell<int>("DEST_AIRPORT_ID", i);
            info.year = doc.GetCell<int>("YEAR", i);
            info.quarter = doc.GetCell<int>("QUARTER", i);
            info.month = doc.GetCell<int>("MONTH", i);

            airports.insert(info.origin_airport_id);
            airports.insert(info.dest_airport_id);

            graph[info.origin_airport_id].emplace_back(info.dest_airport_id, info);
        }
    } catch (const std::exception& e) {
        throw AirlinesListException("Error of read CSV-file: " + std::string(e.what()));
    }
}


void GL::AirlinesList::clear() noexcept {
    graph.clear();
    airports.clear();
}


bool GL::AirlinesList::contains_airport(int index) const noexcept {
    return (airports.find(index) != airports.end()); 
}


GL::FlightPath GL::AirlinesList::find_flight_path_between(int origin_index, int dest_index) const {
    if(!this->contains_airport(origin_index)) {
        throw AirlinesListException("Origin airport index not found: " + std::to_string(origin_index));
    }

    if(!this->contains_airport(dest_index)) {
        throw AirlinesListException("Dest airport index not found: " + std::to_string(dest_index));
    }

    FlightPathRow path = dijkstra(origin_index);

    return reconstruct_path(path, dest_index);
}


GL::AirlinesList::FlightPathRow GL::AirlinesList::dijkstra(int origin_index) const noexcept {
    std::unordered_map<int, double> distance;
    std::unordered_map<int, int> parents;

    for (int airport : airports) {
        distance[airport] = std::numeric_limits<double>::max();
        parents[airport] = -1;
    }

    distance[origin_index] = 0;

    using Pair = std::pair<double, int>; // {distance, vertex}
    auto cmp = [](const Pair& a, const Pair& b) { return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> q(cmp);

    q.push({0, origin_index});

    while (!q.empty()) {
        auto [curr_distance, curr_dest] = q.top();
        q.pop();

        if (curr_distance > distance[curr_dest]) continue;
        if (graph.find(curr_dest) == graph.end()) continue;

        for (const auto& [dest_vertex, edge] : graph.at(curr_dest)) {
            if (distance[dest_vertex] > distance[curr_dest] + edge.air_time) {
                distance[dest_vertex] = distance[curr_dest] + edge.air_time;
                parents[dest_vertex] = curr_dest;
                q.push({distance[dest_vertex], dest_vertex});
            }
        }
    }

    return {distance, parents};
}


GL::FlightPath GL::AirlinesList::reconstruct_path(GL::AirlinesList::FlightPathRow& path, int dest_index) const noexcept {
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

