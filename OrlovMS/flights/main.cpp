#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <math.h>

struct Node
{
    float min_time;
    bool processed;
    int prev_node;
};

int main(int argc, char* argv[])
{
    if(argc != 3) {
        std::cout << "Wrong arguments\n";
        std::cout << "Usage: prog <origin code> <destination code>\n";
        return -1;
    }

    //black magic
    std::map<int, std::map<int, float>> flights = {
    #include "database.h"
    };

    std::map<std::string, int> airports = {
    #include "airports.h"
    };

    std::string origin_code(argv[1]);
    if(!airports.count(origin_code)) {
        std::cout << "No origin airport found\n";
        return -1;
    }
    int origin_id = airports[origin_code];

    std::string dest_code(argv[2]);
    if(!airports.count(dest_code)) {
        std::cout << "No destination airport found\n";
        return -1;
    }
    int dest_id = airports[dest_code];

    std::map<int, Node> nodes;
    for(const auto& origin_list : flights) {
        nodes[origin_list.first] = (Node){.min_time = INFINITY, .processed = 0, .prev_node = -1};
    }

    nodes[origin_id].min_time = 0;

    for(;;) {
        float min_time = INFINITY;
        int id = -1;

        for(const auto& node : nodes) {
            if(!std::isinf(node.second.min_time) && !node.second.processed && node.second.min_time < min_time) {
                min_time = node.second.min_time;
                id = node.first;
            }
        }
        if(id == -1) break;

        for(const auto& flight : flights[id]) {
            float new_time = nodes[id].min_time + flight.second;
            if(new_time < nodes[flight.first].min_time) {
                nodes[flight.first].min_time = new_time;
                nodes[flight.first].prev_node = id;
            }
        }
        nodes[id].processed = 1;
    }

    if(std::isinf(nodes[dest_id].min_time)) {
        std::cout << "No path\n";
    }
    else {
        std::cout << "Total flight time: " << nodes[dest_id].min_time << " minutes\n";
    }

    return 0;
}

