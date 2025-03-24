#include "airlines_list.hpp"
#include <climits>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>


std::string get_file_path(const GL::AirlinesList& airlines_list) {
    std::string file_path;
    bool file_path_is_correct = false;

    while(!file_path_is_correct) {
        std::cin >> file_path;
        std::fstream file(file_path);

        if(!file.is_open()) {
            std::cout << "File path not found, please try again\n";
            continue;
        } else {
            file.close();
            file_path_is_correct = true;
        }
    }

    return file_path;
}


int get_airport_index(const GL::AirlinesList& airlines_list) {
    int index = 0;
    bool index_is_correct = false;

    while(!index_is_correct) {
        std::cin >> index;

        if(airlines_list.contains_airport(index) == false) {
            std::cout << "Index not found, please, try again\n";
        } else {
            index_is_correct = true;
        }
    }

    return index;
}


void print_flight_path(const GL::FlightPath& flight_path) {
    const auto& path = flight_path.first;
    const auto& times = flight_path.second;

    if (path.empty()) {
        std::cout << "No path found.\n";
        return;
    }

    std::cout << "Origin airport index: " << path[0] << "\n";
    double total_air_time = 0.0;

    for (size_t i = 1; i < path.size(); ++i) {
        std::cout << "Next airport index: " << path[i] << "\n";
        std::cout << "Air time: " << times[i - 1] << "\n";

        total_air_time += times[i - 1];
    }

    std::cout << "Total air time: " << total_air_time << "\n";
}


int main (int argc, char *argv[]) {
    int origin_index = 0, dest_index = 0;
    std::string file_path;
    GL::AirlinesList& airlines_list = GL::AirlinesList::get_instance();
    GL::FlightPath flight_path;

    try {
        std::cout << "Please, write the path to csv-file\n";
        file_path = get_file_path(airlines_list);
        
        std::cout << "Please, wait...\n";
        airlines_list.load_data(file_path);

        std::cout << "Please, write the origin airport index\n";
        origin_index = get_airport_index(airlines_list); 

        std::cout << "Please, write the dest airport index\n";
        dest_index = get_airport_index(airlines_list);
        
        if(origin_index == dest_index) {
            std::cout << "Origin airport index not be equal dest airport index\n";
            return -1;
        }

        flight_path = airlines_list.find_flight_path_between(origin_index, dest_index);
        print_flight_path(flight_path);
    } catch (GL::AirlinesListException& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
