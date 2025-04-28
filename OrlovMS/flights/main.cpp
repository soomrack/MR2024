#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <math.h>


using airportCode = std::string;
using airportId = int;


struct Node
{
    float min_time;
    bool processed;
    airportId prev_node;
};


std::vector<std::string> split(std::string str)
{
    std::vector<std::string> elements = {""};
    unsigned int element_cnt = 0;
    bool is_quoted = false;

    for(char ch : str) {
        switch(ch)
        {
        case ',':
            if(!is_quoted) {
                element_cnt++;
                elements.push_back("");
            }
            break;
        case '"':
            is_quoted = !is_quoted;

        default:
            elements[element_cnt].push_back(ch);
            break;
        }
    }

    return elements;
}


int main(int argc, char* argv[])
{
    if(argc != 3) {
        std::cout << "Wrong arguments\n";
        std::cout << "Usage: prog <origin code> <destination code>\n";
        return -1;
    }

    std::map<airportId, std::map<airportId, float>> flights;

    std::string csv_str;

    std::ifstream database_csv("database.csv");
    if(!database_csv.is_open()) {
        std::cout << "No database file\n";
        return -1;
    }

    while(std::getline(database_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        airportId origin_id = std::stoi(elements[0]);
        airportId dest_id = std::stoi(elements[1]);
        float air_time = std::stof(elements[2]);

        flights[origin_id][dest_id] = air_time;
    }
    database_csv.close();

    std::map<airportCode, airportId> airports;

    std::ifstream airports_csv("airports.csv");
    if(!airports_csv.is_open()) {
        std::cout << "No airports list\n";
        return -1;
    }

    while(std::getline(airports_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        airportCode airport_code = elements[0];
        airportId airport_id = std::stoi(elements[1]);

        airports[airport_code] = airport_id;
    }
    airports_csv.close();

    airportCode origin_code(argv[1]);
    if(!airports.count(origin_code)) {
        std::cout << "No origin airport found\n";
        return -1;
    }
    airportId origin_id = airports[origin_code];

    airportCode dest_code(argv[2]);
    if(!airports.count(dest_code)) {
        std::cout << "No destination airport found\n";
        return -1;
    }
    airportId dest_id = airports[dest_code];

    std::map<airportId, Node> nodes;
    for(const auto& origin_list : flights) {
        nodes[origin_list.first] = (Node){.min_time = INFINITY, .processed = 0, .prev_node = -1};
    }

    nodes[origin_id].min_time = 0;
    airportId id = origin_id;

    while(id != -1) {
        for(const auto& flight : flights[id]) {
            float new_time = nodes[id].min_time + flight.second;
            if(new_time < nodes[flight.first].min_time) {
                nodes[flight.first].min_time = new_time;
                nodes[flight.first].prev_node = id;
            }
        }
        nodes[id].processed = 1;

        id = -1;
        float min_time = INFINITY;
        for(const auto& node : nodes) {
            if(!std::isinf(node.second.min_time) && !node.second.processed && node.second.min_time < min_time) {
                min_time = node.second.min_time;
                id = node.first;
            }
        }
    }

    if(std::isinf(nodes[dest_id].min_time)) {
        std::cout << "No path\n";
    }
    else {
        std::cout << "Total flight time: " << nodes[dest_id].min_time << " minutes\n";
    }

    return 0;
}

