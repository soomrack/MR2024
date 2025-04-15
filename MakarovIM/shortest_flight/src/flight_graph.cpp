#include "flight_graph.hpp"
#include "csv_flight_parser.hpp"
#include "error_handler.hpp"
#include "utils.hpp"
#include <limits>
#include <queue>
#include <algorithm>
#include <cstdio>
#include <stdexcept>


void FlightGraph::load_from_csv(const std::string& filename) {
    std::vector<FlightRecord> records = CsvFlightParser::parse(filename);

    for (const auto& rec : records) {
        std::string origin_city = StringUtils::normalize_city_name(rec.origin_city);
        std::string dest_city = StringUtils::normalize_city_name(rec.dest_city);

        add_airport_mapping(rec.origin_airport, origin_city);
        add_airport_mapping(rec.dest_airport, dest_city);
    }

    for (const auto& rec : records) {
        if (rec.air_time <= 0) continue;

        std::string origin = StringUtils::normalize_city_name(rec.origin_city);
        std::string dest = StringUtils::normalize_city_name(rec.dest_city);

        if (origin != dest) {
            add_city_connection(origin, dest, rec.air_time);
        }
    }
}


std::pair<int, std::vector<std::string>> FlightGraph::find_shortest_path(
    const std::string& start,
    const std::string& end
) const {

    std::string start_norm = StringUtils::normalize_city_name(start);
    std::string end_norm = StringUtils::normalize_city_name(end);
    
    validate_city(start_norm);
    validate_city(end_norm);

    using pq_node = std::pair<int, std::string>;
    std::priority_queue<pq_node, std::vector<pq_node>, std::greater<>> pq;

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;

    for (const auto& [city, _] : cities) {
        dist[city] = std::numeric_limits<int>::max();
    }

    dist[start_norm] = 0;
    pq.emplace(0, start_norm);

    while (!pq.empty()) {
        auto [current_dist, current] = pq.top();
        pq.pop();

        if (current == end_norm) break;
        if (current_dist > dist[current]) continue;

        for (const auto& [neighbor, time] : cities.at(current).neighbors) {
            if (dist[neighbor] > current_dist + time) {
                dist[neighbor] = current_dist + time;
                prev[neighbor] = current;
                pq.emplace(dist[neighbor], neighbor);
            }
        }
    }

    if (dist[end_norm] == std::numeric_limits<int>::max()) {
        return { -1, {} };
    }

    std::vector<std::string> path;
    for (std::string at = end_norm; !at.empty(); at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    return { dist[end_norm], path };
}

void FlightGraph::add_airport_mapping(const std::string& airport, const std::string& city) {
    airport_to_city[airport] = city;
    cities[city];
}

void FlightGraph::add_city_connection(const std::string& city1, const std::string& city2, int time) {
    auto& c1 = cities[city1].neighbors;
    auto& c2 = cities[city2].neighbors;

    if (auto it = c1.find(city2); it != c1.end()) {
        if (time < it->second) {
            it->second = time;
            c2[city1] = time;
        }
    } else {
        c1[city2] = time;
        c2[city1] = time;
    }
}

void FlightGraph::validate_city(const std::string& city_name) const 
{
    if (!contains_city(city_name)) {
        throw DataError("City not found: " + city_name);
    }
}


bool FlightGraph::contains_city(const std::string& city_name) const 
{
    return cities.find(city_name) != cities.end();
}