#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <ctime>
#include <climits>
#include <iomanip>
#include <locale>
#include <memory>


const int MIN_CONNECTION_TIME = 3600; // Минимальное время между рейсами (можно менять)


class AirportException : public std::exception {
private:
    std::string message;
public:
    AirportException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class FileException : public std::exception {
private:
    std::string message;
public:
    FileException(const std::string& filename, const std::string& msg)
        : message("Ошибка файла (" + filename + "): " + msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};


class Airport {
private:
    int id;
    std::string code;
    std::string city_state;

public:
    Airport() : id(0) {}

    Airport(int id, const std::string& code, const std::string& city_state)
        : id(id), code(code), city_state(city_state) {}

    int get_id() const { return id; }
    const std::string& get_code() const { return code; }
    const std::string& get_city_state() const { return city_state; }

    std::string to_string() const {
        return code + " (" + city_state + ")";
    }
};


class Flight {
private:
    int origin_id;
    int dest_id;
    int distance;

public:
    Flight() : origin_id(0), dest_id(0), distance(0) {}

    Flight(int orig, int dest, int dist)
        : origin_id(orig), dest_id(dest), distance(dist) {}

    int get_origin_id() const { return origin_id; }
    int get_destination_id() const { return dest_id; }
    int get_distance() const { return distance; }
};


class FlightGraph {
private:
    std::unordered_map<int, Airport> airports;
    std::unordered_map<std::string, int> code_to_id;
    std::unordered_map<int, std::vector<Flight>> adjacency_list;

public:
    void load_airports(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw FileException(filename, "Не удалось открыть файл");
        }

        std::string line;
        int line_num = 0;

        while (std::getline(file, line)) {
            line_num++;
            if (line.empty() || line_num == 1) continue;

            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 3) {
                std::cerr << "Пропуск строки " << line_num << ": недостаточно данных\n";
                continue;
            }

            try {
                int id = std::stoi(tokens[1]);
                std::string code = tokens[0];
                std::string city_state = tokens[2];

                airports[id] = Airport(id, code, city_state);
                code_to_id[code] = id;
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка обработки строки " << line_num << ": " << e.what() << "\n";
            }
        }

        std::cout << "Загружено аэропортов: " << airports.size() << std::endl;
    }

    void load_flights(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw FileException(filename, "Не удалось открыть файл");
        }

        std::string line;
        int line_num = 0;

        while (std::getline(file, line)) {
            line_num++;
            if (line.empty() || line_num == 1) continue;

            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() < 3) {
                std::cerr << "Пропуск строки " << line_num << ": недостаточно данных\n";
                continue;
            }

            try {
                int from = std::stoi(tokens[0]);
                int to = std::stoi(tokens[1]);
                int distance = std::stoi(tokens[2]);

                adjacency_list[from].emplace_back(from, to, distance);
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка обработки строки " << line_num << ": " << e.what() << "\n";
            }
        }

        std::cout << "Загружено рейсов: " << line_num - 1 << std::endl;
    }

    const Airport& get_airport_by_id(int id) const {
        auto it = airports.find(id);
        if (it == airports.end()) {
            throw AirportException("Аэропорт с ID " + std::to_string(id) + " не найден");
        }
        return it->second;
    }

    const Airport& get_airport_by_code(const std::string& code) const {
        auto it = code_to_id.find(code);
        if (it == code_to_id.end()) {
            throw AirportException("Аэропорт с кодом " + code + " не найден");
        }
        return get_airport_by_id(it->second);
    }

    const std::vector<Flight>& get_flights_from(int airport_id) const {
        static const std::vector<Flight> empty;
        auto it = adjacency_list.find(airport_id);
        return it != adjacency_list.end() ? it->second : empty;
    }

    bool contains_airport(const std::string& code) const {
        return code_to_id.find(code) != code_to_id.end();
    }
};


class ShortestPathFinder {
public:
    enum SearchCriteria {
        SHORTEST_DISTANCE,
        FEWEST_STOPS
    };

    struct PathNode {
        int airport_id;
        int total_distance;
        int stops;
        std::shared_ptr<PathNode> prev;

        bool operator>(const PathNode& other) const {
            if (total_distance != other.total_distance)
                return total_distance > other.total_distance;
            return stops > other.stops;
        }
    };

    static std::vector<Flight> find_shortest_path(
        const FlightGraph& graph,
        const std::string& start_code,
        const std::string& dest_code,
        SearchCriteria criteria = SHORTEST_DISTANCE) {

        if (!graph.contains_airport(start_code)) {
            throw AirportException("Аэропорт отправления не найден: " + start_code);
        }
        if (!graph.contains_airport(dest_code)) {
            throw AirportException("Аэропорт назначения не найден: " + dest_code);
        }

        int start_id = graph.get_airport_by_code(start_code).get_id();
            int dest_id = graph.get_airport_by_code(dest_code).get_id();

            auto cmp = [](const std::shared_ptr<PathNode>& a, const std::shared_ptr<PathNode>& b) {
            return *a > *b;
        };
        std::priority_queue<std::shared_ptr<PathNode>,
            std::vector<std::shared_ptr<PathNode>>,
            decltype(cmp)> pq(cmp);

        std::unordered_map<int, int> best_distances;
        std::unordered_map<int, int> best_stops;
        std::unordered_map<int, std::shared_ptr<PathNode>> best_paths;

        auto start_node = std::make_shared<PathNode>();
        start_node->airport_id = start_id;
        start_node->total_distance = 0;
        start_node->stops = 0;
        start_node->prev = nullptr;

        pq.push(start_node);
        best_distances[start_id] = 0;
        best_stops[start_id] = 0;

        std::shared_ptr<PathNode> final_node = nullptr;

        while (!pq.empty()) {
            auto current = pq.top();
            pq.pop();

            if (current->airport_id == dest_id) {
                final_node = current;
                break;
            }

            if (current->total_distance > best_distances[current->airport_id]) {
                continue;
            }

            for (const auto& flight : graph.get_flights_from(current->airport_id)) {
                int new_distance = current->total_distance + flight.get_distance();
                int new_stops = current->stops + 1;

                bool should_add = false;
                if (best_distances.find(flight.get_destination_id()) == best_distances.end()) {
                    should_add = true;
                }
                else if (criteria == SHORTEST_DISTANCE && new_distance < best_distances[flight.get_destination_id()]) {
                    should_add = true;
                }
                else if (criteria == FEWEST_STOPS && new_stops < best_stops[flight.get_destination_id()]) {
                    should_add = true;
                }

                if (should_add) {
                    auto new_node = std::make_shared<PathNode>();
                    new_node->airport_id = flight.get_destination_id();
                    new_node->total_distance = new_distance;
                    new_node->stops = new_stops;
                    new_node->prev = current;

                    best_distances[flight.get_destination_id()] = new_distance;
                    best_stops[flight.get_destination_id()] = new_stops;
                    best_paths[flight.get_destination_id()] = new_node;

                    pq.push(new_node);
                }
            }
        }

        if (!final_node) {
            throw AirportException("Путь между аэропортами не найден");
        }

        // path reunite
        std::vector<Flight> path;
        auto node = final_node;
        while (node && node->prev) {
            int dest = node->airport_id;
            int origin = node->prev->airport_id;

            const auto& flights = graph.get_flights_from(origin);
            for (const auto& flight : flights) {
                if (flight.get_destination_id() == dest) {
                    path.push_back(flight);
                    break;
                }
            }

            node = node->prev;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};

void print_path(const FlightGraph& graph, const std::vector<Flight>& path) {
    if (path.empty()) {
        std::cout << "Маршрут не найден." << std::endl;
        return;
    }

    std::cout << "\n=== Найденный маршрут ===" << std::endl;

    int total_distance = 0;
    int total_stops = path.size() - 1;

    for (size_t i = 0; i < path.size(); ++i) {
        const auto& flight = path[i];
        total_distance += flight.get_distance();

        const auto& origin = graph.get_airport_by_id(flight.get_origin_id());
        const auto& dest = graph.get_airport_by_id(flight.get_destination_id());

        std::cout << "\nЭтап " << (i + 1) << ":" << std::endl;
        std::cout << "  Из: " << origin.to_string() << std::endl;
        std::cout << "  В: " << dest.to_string() << std::endl;
        std::cout << "  Расстояние: " << flight.get_distance() << " миль" << std::endl;

        if (i < path.size() - 1) {
            std::cout << "  Пересадка в аэропорту" << std::endl;
        }
    }

    std::cout << "\nИтоги:" << std::endl;
    std::cout << "  Всего перелетов: " << path.size() << std::endl;
    std::cout << "  Всего пересадок: " << total_stops << std::endl;
    std::cout << "  Общее расстояние: " << total_distance << " миль" << std::endl;
}

int main() {

    setlocale(LC_ALL, "Russian");

    try {
        std::cout << "Инициализация данных аэропортов и рейсов..." << std::endl;

        FlightGraph graph;

        
        std::cout << "Загрузка данных аэропортов из airport_mapping.csv..." << std::endl;
        graph.load_airports("Database/airport_mapping.csv");

        std::cout << "Загрузка данных рейсов из airline_graph.csv..." << std::endl;
        graph.load_flights("Database/airline_graph.csv");


        std::string start_code, dest_code;

        std::cout << "\nВведите код аэропорта отправления (например, LAX): ";
        std::getline(std::cin, start_code);

        std::cout << "Введите код аэропорта назначения (например, JFK): ";
        std::getline(std::cin, dest_code);


        std::cout << "\nПоиск кратчайшего пути..." << std::endl;
        auto path = ShortestPathFinder::find_shortest_path(
            graph, start_code, dest_code, ShortestPathFinder::SHORTEST_DISTANCE);


        print_path(graph, path);
    }
    catch (const FileException& e) {
        std::cerr << "Ошибка файла: " << e.what() << std::endl;
        return 1;
    }
    catch (const AirportException& e) {
        std::cerr << "Ошибка аэропорта: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}