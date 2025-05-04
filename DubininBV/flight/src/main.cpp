#include "Parser.h"
#include "PathFinder.h"
#include <iostream>
#include <string>

int main() {
    Parser parser;
    PathFinder path_finder;

    std::string origin_city;
    std::string destination_city;

    std::cout << "Write origin city: ";
    std::getline(std::cin, origin_city);
    std::cout << "Write destination city: ";
    std::getline(std::cin, destination_city);
    
    std::vector<Flight> flights = parser.parse_data("data.csv");
    
    std::cout << "Shortest path from '" << origin_city 
              << "' to '" << destination_city << "' is:\n";
    std::cout << path_finder.find_path(origin_city, destination_city, flights);

    return 0; 
}
