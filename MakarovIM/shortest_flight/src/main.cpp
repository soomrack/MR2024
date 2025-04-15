#include <iostream>
#include "flight_graph.hpp" 
#include "logger.hpp"
#include "csv_flight_parser.hpp"
#include "error_handler.hpp"

int main(int argc, char* argv[]) {
    Logger& logger = Logger::get_instance();
    logger.initialize("../logs/server.log");

    try {
        logger.log(LogLevel::INFO, "main", "Program started!");

        if (argc != 3) {
            throw DataError("Usage: ./shortest_flight <start_city> <end_city>");
        }

        FlightGraph graph;
        graph.load_from_csv("/mnt/c/Users/im.makarov/Desktop/pr/my/practice/cpp_lib/shortest_flight/data/T_T100_SEGMENT_ALL_CARRIER.csv");

        const std::string start = argv[1];
        const std::string end = argv[2];

        auto [total_time, path] = graph.find_shortest_path(start, end);

        if (total_time == -1) {
            std::cout << "No available route between " << start << " and " << end << "\n";
            logger.log(LogLevel::INFO, "main", "Program has ended");
            return 0;
        }

        std::cout << "Fastest route from " << start << " to " << end << ":\n";
        std::cout << "Total flight time: " << total_time << " minutes\n";
        std::cout << "Route:\n";

        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << " ↓\n";
            std::cout << "├─ " << path[i];
        }
        std::cout << "\n";

    }
    catch (const std::exception& e) {
        logger.log(LogLevel::ERROR, "main",
            std::string("Unexpected ERROR: ") + e.what());
        throw InternalError("Unexpected ERROR: ");
    }

    logger.log(LogLevel::INFO, "main", "Program has ended");
    return 0;
}