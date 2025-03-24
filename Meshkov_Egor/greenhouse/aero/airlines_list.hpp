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


using AirportID = int;
using AirTime = double;
using FlightPath = std::pair<std::vector<AirportID>, std::vector<AirTime>>;


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

    using GraphEdge = std::vector<std::pair<AirportID, FlightInfo>>;
    using Graph = std::map<AirportID, GraphEdge>;
    using Airports = std::unordered_set<AirportID>;

    Graph graph;
    Airports airports;
public:
    FlightPath find_flight_path_between(const AirportID origin_index, const AirportID dest_index) const;
public:
    static AirlinesList& get_instance() noexcept;
    void load_data(const std::string& file_path);
    void clear() noexcept;
    bool contains_airport(const AirportID index) const noexcept;
    
    using FlightPathRow = std::pair<std::unordered_map<AirportID, AirTime>, std::unordered_map<AirportID, AirportID>>;
private:
    AirlinesList() : graph(), airports() {}
    AirlinesList(const AirlinesList&) = delete;
    AirlinesList(AirlinesList&&) = delete;
    ~AirlinesList() = default;

    std::vector<std::string> parse_csv_line(const std::string& line);
    FlightPathRow dijkstra(const AirportID origin_index) const noexcept;
    FlightPath reconstruct_path(FlightPathRow& path, const AirportID dest_index) const noexcept;
};


};
