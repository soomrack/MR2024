#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <stdexcept>


//Хранение информации о маршруте
struct Route {
    std::string destination;//принимающий аэропорт
    int airtime;//время полета
};


std::unordered_map<std::string, std::vector<Route>> flight_graph;


void load_graph(const std::string& file_path) {
    flight_graph.clear();

    std::ifstream input_file(file_path);
    if (!input_file.is_open()) {
        throw std::runtime_error("Error: Failed to open the file: " + file_path);
    }


    std::string header_line;
    std::getline(input_file, header_line);//пропуск


    std::string data_line;
    while (std::getline(input_file, data_line)) {
        std::stringstream ss(data_line);
        std::string origin, dest, airtime_str;


        // код аэропорта отправления, назначения, время полета
        std::getline(ss, origin, ',');
        std::getline(ss, dest, ',');
        std::getline(ss, airtime_str);


        try {
            flight_graph[origin].push_back({ dest, std::stoi(airtime_str) });//добавляем маршрут
            flight_graph[dest];
        }
    }
}


// Дейкстра
void find_shortest_path(const std::string& start_airport, const std::string& end_airport) {
    typedef std::string AirportCode;
    typedef std::pair<int, AirportCode> TimedAirport;
    typedef std::priority_queue<TimedAirport, std::vector<TimedAirport>, std::greater<>> MinHeap;
    typedef std::unordered_map<AirportCode, int> TimeTable;
    typedef std::unordered_map<AirportCode, AirportCode> PathMap;


    //  минимальное время до каждого аэропорта
    TimeTable total_time;
    //  предыдущий аэропорт в кратчайшем пути
    PathMap previous;
    //  приоритетная очередь по времени
    MinHeap pq;


    // - бесконечное время 
    for (const auto& [airport, _] : flight_graph) {
        total_time[airport] = std::numeric_limits<int>::max();
    }

    total_time[start_airport] = 0;
    pq.push({ 0, start_airport });

    while (!pq.empty()) {
        TimedAirport current = pq.top();
        pq.pop();

        int current_time = current.first;
        AirportCode current_airport = current.second;

        if (current_airport == end_airport) break;

        if (current_time > total_time[current_airport]) continue;

        for (const Route& route : flight_graph[current_airport]) {
            int new_time = current_time + route.airtime;

            if (new_time < total_time[route.destination]) {
                total_time[route.destination] = new_time;
                previous[route.destination] = current_airport;
                pq.push({ new_time, route.destination });
            }
        }
    }

    if (total_time[end_airport] == std::numeric_limits<int>::max()) {
        throw std::runtime_error("There is no path between " + start_airport + " and " + end_airport);
    }

    // Восстанавливаем путь
    std::vector<AirportCode> path;
    for (AirportCode airport = end_airport; !airport.empty(); airport = previous[airport]) {
        path.push_back(airport);
    }
    std::reverse(path.begin(), path.end());


    std::cout << "\nShortest route (" << total_time[end_airport] << " minutes):\n";
    for (std::size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i];
        if (i != path.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
}


void print_available_airports() {
    std::cout << "\nAvailable airports in the database:\n";
    int count = 0;
    for (const auto& [airport, _] : flight_graph) {
        std::cout << airport << " ";
        if (++count % 10 == 0) std::cout << std::endl;
    }
    std::cout << "\nAll: " << count << " airports\n";
}


int main() {
    try {
        load_graph("filtered_data.csv");

        print_available_airports();

        std::string start, end;
        std::cout << "\nEnter the airport code of departure: ";
        std::cin >> start;
        std::cout << "Enter the destination airport code: ";
        std::cin >> end;

        std::transform(start.begin(), start.end(), start.begin(), ::toupper);
        std::transform(end.begin(), end.end(), end.begin(), ::toupper);

        if (!flight_graph.count(start)) {
            throw std::runtime_error("Departure airport '" + start + "' not found in the database");
        }
        if (!flight_graph.count(end)) {
            throw std::runtime_error("Destination airport '" + end + "' not found in the database");
        }

        find_shortest_path(start, end);
        return 0;

    }
}