#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>


const int MIN_CONNECTION_TIME = 3600;


class BaseException : public std::exception {
protected:
    std::string message;

public:
    BaseException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class FileException : public BaseException {
public:
    FileException(const std::string& filename, const std::string& msg)
        : BaseException("Ошибка файла (" + filename + "): " + msg) {
    }
};

class ParseException : public BaseException {
public:
    ParseException(const std::string& data, const std::string& msg)
        : BaseException("Ошибка парсинга (" + data + "): " + msg) {
    }
};

class AlgorithmException : public BaseException {
public:
    AlgorithmException(const std::string& msg)
        : BaseException("Ошибка алгоритма: " + msg) {
    }
};


class airport {
private:
    int index;
    std::string name;

public:
    airport();
    airport(int index, const std::string& name);

    int get_index() const;
    const std::string& get_name() const;

    std::string to_string() const;
    bool operator==(const airport& other) const;
    bool operator!=(const airport& other) const;

    static airport from_csv(const std::string& csv_line);
};

airport::airport() : index(0), name() {}

airport::airport(int index, const std::string& name) : index(index), name(name) {}

int airport::get_index() const { return index; }

const std::string& airport::get_name() const { return name; }

std::string airport::to_string() const {
    return "Аэропорт " + std::to_string(index) + ": " + name;
}

bool airport::operator==(const airport& other) const {
    return index == other.index;
}

bool airport::operator!=(const airport& other) const {
    return !(*this == other);
}

airport airport::from_csv(const std::string& csv_line) {
    std::istringstream iss(csv_line);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 2) {
        throw ParseException(csv_line, "Недостаточно данных для создания аэропорта");
    }

    int index = std::stoi(tokens[0]);
    std::string name = tokens[1];

    return airport(index, name);
}


class flight {
private:
    airport origin;
    airport destination;
    time_t departure_time;
    time_t total_time;
    time_t flight_time;
    double distance;

public:
    flight(const airport& orig, const airport& dest, time_t depart, time_t total, time_t flight, double dist);

    const airport& get_origin() const;
    const airport& get_destination() const;
    time_t get_departure_time() const;
    time_t get_total_time() const;
    time_t get_flight_time() const;
    double get_distance() const;

    void print() const;
};

flight::flight(const airport& orig, const airport& dest, time_t depart, time_t total, time_t flight, double dist)
    : origin(orig), destination(dest), departure_time(depart), total_time(total),
    flight_time(flight), distance(dist) {
}

const airport& flight::get_origin() const { return origin; }

const airport& flight::get_destination() const { return destination; }

time_t flight::get_departure_time() const { return departure_time; }

time_t flight::get_total_time() const { return total_time; }

time_t flight::get_flight_time() const { return flight_time; }

double flight::get_distance() const { return distance; }

void flight::print() const {
    std::cout << "Рейс из " << origin.get_index() << " (" << origin.get_name() << ") в "
        << destination.get_index() << " (" << destination.get_name() << ")"
        << " | Время вылета: " << ctime(&departure_time)
        << " | Время в пути: " << total_time / 60 << " минут"
        << " | Время полета: " << flight_time / 60 << " минут"
        << " | Расстояние: " << distance << " миль" << std::endl;
}


class file_reader {
private:
    std::unordered_map<std::string, int> header_map;

    std::unordered_map<std::string, int> parse_header(const std::string& header_line);
    flight parse_flight_line(const std::string& line, const std::unordered_map<std::string, int>& header_map);

public:
    std::vector<flight> read_flights_from_file(const std::string& filename);
};

std::unordered_map<std::string, int> file_reader::parse_header(const std::string& header_line) {
    std::unordered_map<std::string, int> map;
    std::istringstream ss(header_line);
    std::string token;
    int index = 0;
    while (std::getline(ss, token, ',')) {
        map[token] = index++;
    }
    return map;
}

flight file_reader::parse_flight_line(const std::string& line, const std::unordered_map<std::string, int>& header_map) {
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 52) {
        throw ParseException(line, "Недостаточно данных в строке");
    }

    try {
        int originindex = std::stoi(tokens[header_map.at("ORIGIN")]);
        int dest_index = std::stoi(tokens[header_map.at("DEST")]);
        time_t departure_time = std::stol(tokens[header_map.at("DEPARTURES_SCHEDULED")]);
        time_t total_time = std::stol(tokens[header_map.at("RAMP_TO_RAMP")]);
        time_t flight_time = std::stol(tokens[header_map.at("AIR_TIME")]);
        double distance = std::stod(tokens[header_map.at("DISTANCE")]);

        std::string originname = tokens[header_map.at("originCITY_NAME")];
        std::string dest_name = tokens[header_map.at("DEST_CITY_NAME")];

        airport origin(originindex, originname);
        airport dest(dest_index, dest_name);

        return flight(origin, dest, departure_time, total_time, flight_time, distance);
    }
    catch (const std::exception& e) {
        throw ParseException(line, "Ошибка преобразования данных");
    }
}

std::vector<flight> file_reader::read_flights_from_file(const std::string& filename) {
    std::vector<flight> flights;
    std::ifstream infile(filename);
    if (!infile) {
        throw FileException(filename, "Не удалось открыть файл");
    }

    std::string header_line;
    if (!std::getline(infile, header_line)) {
        throw FileException(filename, "Не удалось прочитать заголовок");
    }
    header_map = parse_header(header_line);

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        try {
            flight fl = parse_flight_line(line, header_map);
            flights.push_back(fl);
        }
        catch (const ParseException& e) {
            std::cerr << "Предупреждение: " << e.what() << std::endl;
        }
    }
    return flights;
}


class flight_graph {
private:
    std::unordered_map<int, std::vector<flight>> adj_list;

public:
    void add_flight(const flight& fl);
    const std::vector<flight>& get_flights_from(const airport& ap) const;
    void sort_flights();
};

void flight_graph::add_flight(const flight& fl) {
    adj_list[fl.get_origin().get_index()].push_back(fl);
}

const std::vector<flight>& flight_graph::get_flights_from(const airport& ap) const {
    auto it = adj_list.find(ap.get_index());
    return (it != adj_list.end()) ? it->second : std::vector<flight>();
}

void flight_graph::sort_flights() {
    for (auto& pair : adj_list) {
        std::sort(pair.second.begin(), pair.second.end(),
            [](const flight& a, const flight& b) {
                return a.get_departure_time() < b.get_departure_time();
            });
    }
}


class shortest_path_finder {
private:
    const flight_graph& graph;

public:
    shortest_path_finder(const flight_graph& g);
    std::vector<flight> find_shortest_path(const airport& start, const airport& destination, time_t user_departure_time);
};

shortest_path_finder::shortest_path_finder(const flight_graph& g) : graph(g) {}

std::vector<flight> shortest_path_finder::find_shortest_path(const airport& start, const airport& destination, time_t user_departure_time) {
    std::unordered_map<int, time_t> best_time;
    std::unordered_map<int, flight> prev_flight;
    using node = std::pair<time_t, int>;
    auto cmp = [](const node& a, const node& b) { return a.first > b.first; };
    std::priority_queue<node, std::vector<node>, decltype(cmp)> pq(cmp);

    best_time[start.get_index()] = user_departure_time;
    pq.push({ user_departure_time, start.get_index() });

    bool found = false;

    while (!pq.empty() && !found) {
        auto current_pair = pq.top();
        pq.pop();
        time_t current_time = current_pair.first;
        int current_ap = current_pair.second;

        if (current_ap == destination.get_index()) {
            found = true;
            break;
        }

        if (current_time > best_time[current_ap])
            continue;

        for (const auto& fl : graph.get_flights_from(airport(current_ap, ""))) {
            time_t depart = std::max(current_time + MIN_CONNECTION_TIME, fl.get_departure_time());
            time_t arrive = depart + fl.get_total_time();

            if (!best_time.count(fl.get_destination().get_index()) || arrive < best_time[fl.get_destination().get_index()]) {
                best_time[fl.get_destination().get_index()] = arrive;
                prev_flight[fl.get_destination().get_index()] = fl;
                pq.push({ arrive, fl.get_destination().get_index() });
            }
        }
    }

    if (!found) {
        throw AlgorithmException("Не удалось найти маршрут до целевого аэропорта");
    }

    std::vector<flight> path;
    int curr = destination.get_index();
    while (curr != start.get_index()) {
        if (!prev_flight.count(curr)) {
            throw AlgorithmException("Ошибка восстановления пути");
        }
        path.push_back(prev_flight[curr]);
        curr = prev_flight[curr].get_origin().get_index();
    }
    std::reverse(path.begin(), path.end());
    return path;
}


bool get_user_input(airport& start_airport, airport& destinationairport, time_t& user_departure_time) {
    std::cout << "Введите код аэропорта отправления: ";
    std::string start_code;
    if (!std::getline(std::cin, start_code) || start_code.empty()) {
        std::cerr << "Неверный ввод кода аэропорта отправления." << std::endl;
        return false;
    }

    std::cout << "Введите код аэропорта назначения: ";
    std::string dest_code;
    if (!std::getline(std::cin, dest_code) || dest_code.empty()) {
        std::cerr << "Неверный ввод кода аэропорта назначения." << std::endl;
        return false;
    }

    std::cout << "Введите дату и время вылета (ГГГГ-ММ-ДД ЧЧ:ММ): ";
    std::string date_time_str;
    if (!std::getline(std::cin, date_time_str) || date_time_str.empty()) {
        std::cerr << "Неверный ввод даты/времени." << std::endl;
        return false;
    }

    std::istringstream ss(date_time_str);
    std::tm tm_val = {};
    ss >> std::get_time(&tm_val, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        std::cerr << "Ошибка: неверный формат даты/времени." << std::endl;
        return false;
    }
    user_departure_time = mktime(&tm_val);

    start_airport = airport(0, start_code);
    destinationairport = airport(0, dest_code);

    return true;
}


void print_shortest_path(const std::vector<flight>& path, time_t user_departure_time) {
    if (path.empty()) {
        std::cout << "Маршрут не найден." << std::endl;
        return;
    }

    std::cout << "Найденный маршрут:" << std::endl;
    double total_distance = 0.0;
    time_t total_time = 0;
    std::vector<std::string> layovers;

    for (size_t i = 0; i < path.size(); ++i) {
        const auto& fl = path[i];
        fl.print();
        total_distance += fl.get_distance();
        total_time += fl.get_total_time();

        if (i < path.size() - 1) {
            layovers.push_back(fl.get_destination().get_name());
        }
    }

    if (!layovers.empty()) {
        std::cout << "Пересадки в аэропортах: ";
        for (size_t i = 0; i < layovers.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << layovers[i];
        }
        std::cout << std::endl;
    }

    std::cout << "Общее расстояние: " << total_distance << " миль" << std::endl;
    std::cout << "Общее время в пути: "
        << total_time / 3600 << " часов "
        << (total_time % 3600) / 60 << " минут." << std::endl;
}


int main() {
    try {
        file_reader reader;
        auto flights = reader.read_flights_from_file("Graph.csv");

        if (flights.empty()) {
            std::cerr << "Файл не содержит данных о рейсах." << std::endl;
            return 1;
        }

        flight_graph graph;
        for (const auto& fl : flights) {
            graph.add_flight(fl);
        }
        graph.sort_flights();

        airport start, destination;
        time_t departure_time;

        if (!get_user_input(start, destination, departure_time)) {
            std::cerr << "Ошибка ввода данных." << std::endl;
            return 1;
        }

        shortest_path_finder spf(graph);
        auto path = spf.find_shortest_path(start, destination, departure_time);

        print_shortest_path(path, departure_time);

    }
    catch (const FileException& e) {
        std::cerr << "Ошибка файла: " << e.what() << std::endl;
        return 1;
    }
    catch (const ParseException& e) {
        std::cerr << "Ошибка парсинга: " << e.what() << std::endl;
        return 1;
    }
    catch (const AlgorithmException& e) {
        std::cerr << "Ошибка алгоритма: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Неизвестная ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}