#pragma once
#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <exception>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include <limits>

using AirportID = int;
using AirTime = float;
using Path = std::pair<std::vector<AirportID>, std::vector<AirTime>>;

class Flights {
private:

struct NodeData {
        int parent = -1;
        float distance = std::numeric_limits<float>::max();
    };
using GraphEdge = std::vector<std::pair<AirportID, AirTime>>;
using Graph = std::unordered_map<AirportID, GraphEdge>;
using Node = std::unordered_set<AirportID>;
using Shortestpaths = std::unordered_map<AirportID, NodeData>;
Graph graph;
Node node;

public:

Flights() : graph(), node() {}
Flights(Graph mygraph, Node mynode): graph(mygraph), node(mynode){};
 
Flights(const Flights&) = delete;
Flights(Flights&&) = delete;
~Flights() = default;

std::vector<std::string> split_csv_line(const std::string& line);
void extract_flight_data_csv();
void printGraph() const;
Shortestpaths dijkstra_algorithm(const AirportID origin_index) const noexcept;
Path restorePath(Shortestpaths& shortpath, const AirportID dest_index) const noexcept;
Path find_the_shortest_path(const AirportID origin_index, const AirportID dest_index) const;

};


class FlightsException : public std::exception { 
    private:
        std::string message;
    public:
        explicit FlightsException (const std::string& msg) : message(msg) {} 
        
        const char* what() const noexcept override{
            return message.c_str();
        }
    };