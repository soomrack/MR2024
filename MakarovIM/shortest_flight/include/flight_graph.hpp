#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class FlightGraph {
public:
    void load_from_csv(const std::string& filename);
    std::pair<int, std::vector<std::string>> find_shortest_path(
        const std::string& start_city,
        const std::string& end_city
    ) const;

    bool contains_city(const std::string& city_name) const;
    const std::unordered_set<std::string>& get_cities() const;

private:
    struct CityConnection {
        std::unordered_map<std::string, int> neighbors; // city -> travel_time
    };

    std::unordered_map<std::string, CityConnection> cities;
    std::unordered_map<std::string, std::string> airport_to_city;

    void add_airport_mapping(const std::string& airport_code, const std::string& city_name);
    void add_city_connection(const std::string& city1, const std::string& city2, int travel_time);
    void validate_city(const std::string& city_name) const;
};
