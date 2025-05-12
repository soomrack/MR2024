
#include "Flights.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <sstream>
#include <limits>
#include <algorithm>

void Flights::printGraph() const {
    std::cout << "=== Graph Contents ===" << std::endl;
    std::cout << "Total nodes: " << node.size() << std::endl;
    std::cout << "Total edges: ";

    // Подсчет общего количества ребер
    size_t total_edges = 0;
    for (const auto& [from, edges] : graph) {
        total_edges += edges.size();
    }
    std::cout << total_edges << std::endl << std::endl;

    
    for (const auto& [from_airport, edges] : graph) {
        std::cout << "Airport " << from_airport << " has flights to:" << std::endl;
        
        for (const auto& [to_airport, air_time] : edges) {
            std::cout << "  -> " << to_airport 
                      << " (flight time: " << air_time << " hours)" << std::endl;
        }
        
        std::cout << std::endl;
    }
}
 
std::vector<std::string> Flights::split_csv_line(const std::string& line) 
{   
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
   
    std::string processed_line = line;
    if (!line.empty() && line.front() == '"' && line.back() == '"') {
        processed_line = line.substr(1, line.size() - 2);
    }
    for (char c : processed_line) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    if (!token.empty())
    tokens.push_back(token);
    
    return tokens;
}
 

void Flights::extract_flight_data_csv() 
{
    std::string file_path = "C:\\Users\\krola\\Desktop\\par.csv";
    bool file_path_is_correct = false;
    std::fstream file;
    

    while(!file_path_is_correct) {
        //std::cin >> file_path;
        file.open(file_path);

          if (!file.is_open()) {
           std::cout<< "Сouldn't open the file,please try again"<< std::endl; 
           continue;
        }
            file_path_is_correct = true;
        
    }
   
    
    std::string line;

    if (!std::getline(file, line)) {
        throw FlightsException("Empty file ");
    }

    std::unordered_map<std::string, size_t> column_indexes;
    std::vector<std::string> headers = split_csv_line(line);
    for (size_t i = 0; i < headers.size(); ++i) {
        column_indexes[headers[i]] = i;
    }


    size_t row_number = 0;
    while (std::getline(file, line)) {
        row_number++;
        std::vector<std::string> row = split_csv_line(line);

        if (row.size() < column_indexes.size()) {
            throw FlightsException("Invalid row in CSV file at line " + std::to_string(row_number));
        }

        AirTime air_time;
        AirportID origin_airport_id ;
        AirportID dest_airport_id;

        try {
             air_time = std::stod(row[column_indexes.at("AIR_TIME")]);
             origin_airport_id = std::stoi(row[column_indexes.at("ORIGIN_AIRPORT_ID")]);
             dest_airport_id = std::stoi(row[column_indexes.at("DEST_AIRPORT_ID")]);
            std::cout << "Row " << row_number << ": "
         << "AIR_TIME=" << air_time << ", "
         << "ORIGIN=" << origin_airport_id << ", "
         << "DEST=" << dest_airport_id << "\n";
        } catch (const std::invalid_argument& e) {
            throw FlightsException("Error parsing data at line " + std::to_string(row_number) + ": " + e.what());
        }

        node.insert(origin_airport_id);
        node.insert(dest_airport_id);

        graph[origin_airport_id].emplace_back(dest_airport_id, air_time);
    }

    file.close();
    Flights::printGraph(); 
}


Flights::Shortestpaths Flights::dijkstra_algorithm(const AirportID origin_index) const noexcept{
 
   Shortestpaths node_info;

    for (const auto& id : node) {
        node_info[id]; 
    }
    node_info[origin_index].distance = 0;

    using Pair = std::pair<float, int>; 
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> min_heap;
 

    min_heap.push({0, origin_index});

    while (!min_heap.empty()) {
        auto [new_time, new_id] = min_heap.top();
        min_heap.pop();

         
        if (new_time > node_info[new_id].distance) {
            continue;
        }

       
        if (graph.find(new_id) == graph.end()) {
            continue;
        }
            for (const auto& [neighbour, airtime] : graph.at(new_id)) {
                if (node_info[neighbour].distance > node_info[new_id].distance + airtime) {
                    node_info[neighbour].distance = node_info[new_id].distance + airtime;
                    node_info[neighbour].parent = new_id;
                    min_heap.push({node_info[neighbour].distance, neighbour});
                }
            }
    }
    
  for (const auto& id : node) {
       std::cout << "Node " << id 
              << ": distance = " << node_info[id].distance 
              << ", parent = " << node_info[id].parent << std::endl;
    }
    
    return node_info;
}


Path Flights::restorePath(Flights::Shortestpaths& shortpath, const AirportID dest_index) const noexcept
{
    Path  nul_path = {};
    if (shortpath.find(dest_index) == shortpath.end() 
       || shortpath[dest_index].distance == std::numeric_limits<AirTime>::max())  {
            return  nul_path ;
    }
    Path path;
    int cur_index = dest_index;
    
    while(cur_index !=-1) {
       
        path.first.push_back(cur_index);
        path.second.push_back(shortpath[cur_index].distance);
        cur_index = shortpath[cur_index].parent; 
        
    }  

    std::reverse(path.first.begin(), path.first.end());
    std::reverse(path.second.begin(), path.second.end());

    return path;
}


Path Flights::find_the_shortest_path(const AirportID origin_index, const AirportID dest_index) const
{
    Path nul_path = {};

    if ((graph.find(origin_index) == graph.end())) {
            throw FlightsException("Origin airport index not found");
        }
    if ((graph.find(dest_index) == graph.end())) {
            throw FlightsException("Destination airport index not found");
        }

    Flights::Shortestpaths shortspaths = dijkstra_algorithm(origin_index);
    Path path = restorePath(shortspaths,dest_index);

    if (path == nul_path) {
        throw FlightsException("Path construction error");
    }

    return path;
}


