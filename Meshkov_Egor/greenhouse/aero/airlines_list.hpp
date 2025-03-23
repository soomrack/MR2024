#pragma once
#include <cstdlib>
#include <exception>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


namespace GL {


class AirlinesListException : std::exception {
private:
    std::string msg_err;
public:
    explicit AirlinesListException(const std::string& msg) : msg_err(msg) {}

    const char* what() const noexcept {
        return msg_err.c_str();
    }
};


using FlightPath = std::pair<std::vector<int>, std::vector<double>>; // parents && distance


class AirlinesList {
private:
    struct FlightInfo {
        double air_time;
        int origin_airport_id;
        int dest_airport_id;
        int year;
        int quarter;
        int month;
    };

    using Graph = std::map<int, std::vector<std::pair<int, FlightInfo>>>; // airport index && vector of pair "vertex - edge"
    using Airports = std::unordered_set<int>;

    Graph graph;
    Airports airports;
public:
    static AirlinesList& get_instance() noexcept;
    void load_data(const std::string& file_path);
    void clear() noexcept;
    bool contains_airport(int index) const noexcept;
    FlightPath find_flight_path_between(int origin_index, int dest_index) const;
    
    using FlightPathRow = std::pair<std::unordered_map<int, double>, std::unordered_map<int, int>>;
private:
    AirlinesList() : graph(), airports() {}
    AirlinesList(const AirlinesList&) = delete;
    AirlinesList(AirlinesList&&) = delete;
    ~AirlinesList() = default;

    std::vector<std::string> parse_csv_line(const std::string& line);
    FlightPathRow dijkstra(int origin_index) const noexcept;
    FlightPath reconstruct_path(FlightPathRow& path, int dest_index) const noexcept;
};


};
