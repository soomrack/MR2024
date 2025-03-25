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
        : BaseException("������ ����� (" + filename + "): " + msg) {
    }
};

class ParseException : public BaseException {
public:
    ParseException(const std::string& data, const std::string& msg)
        : BaseException("������ �������� (" + data + "): " + msg) {
    }
};

class AlgorithmException : public BaseException {
public:
    AlgorithmException(const std::string& msg)
        : BaseException("������ ���������: " + msg) {
    }
};


class airport {
private:
    int index_;
    std::string name_;

public:
    airport() : index_(0), name_() {}
    airport(int index, const std::string& name) : index_(index), name_(name) {}

    int get_index() const { return index_; }
    const std::string& get_name() const { return name_; }

    std::string to_string() const {
        return "�������� " + std::to_string(index_) + ": " + name_;
    }

    bool operator==(const airport& other) const { return index_ == other.index_; }
    bool operator!=(const airport& other) const { return !(*this == other); }

    static airport from_csv(const std::string& csv_line) {
        std::istringstream iss(csv_line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 2) {
            throw ParseException(csv_line, "������������ ������ ��� �������� ���������");
        }

        int index = std::stoi(tokens[0]);
        std::string name = tokens[1];

        return airport(index, name);
    }
};


class flight {
private:
    airport origin_;
    airport destination_;
    time_t departure_time_;
    time_t total_time_;
    time_t flight_time_;
    double distance_;

public:
    flight() : origin_(), destination_(), departure_time_(0), total_time_(0), flight_time_(0), distance_(0) {}
    flight(const airport& orig, const airport& dest, time_t depart, time_t total, time_t flight, double dist)
        : origin_(orig), destination_(dest), departure_time_(depart), total_time_(total),
        flight_time_(flight), distance_(dist) {
    }

    const airport& get_origin() const { return origin_; }
    const airport& get_destination() const { return destination_; }
    time_t get_departure_time() const { return departure_time_; }
    time_t get_total_time() const { return total_time_; }
    time_t get_flight_time() const { return flight_time_; }
    double get_distance() const { return distance_; }

    void print() const {
        char time_buf[26];
        ctime_s(time_buf, sizeof(time_buf), &departure_time_);

        std::cout << "���� �� " << origin_.get_index() << " (" << origin_.get_name() << ") � "
            << destination_.get_index() << " (" << destination_.get_name() << ")"
            << " | ����� ������: " << time_buf
            << " | ����� � ����: " << total_time_ / 60 << " �����"
            << " | ����� ������: " << flight_time_ / 60 << " �����"
            << " | ����������: " << distance_ << " ����" << std::endl;
    }
};


class file_reader {
private:
    std::unordered_map<std::string, int> header_map;

    std::unordered_map<std::string, int> parse_header(const std::string& header_line) {
        std::unordered_map<std::string, int> map;
        std::istringstream ss(header_line);
        std::string token;
        int index = 0;
        while (std::getline(ss, token, ',')) {
            map[token] = index++;
        }
        return map;
    }

    flight parse_flight_line(const std::string& line, const std::unordered_map<std::string, int>& header_map) {
        std::istringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 52) {
            throw ParseException(line, "������������ ������ � ������");
        }

        try {
            int origin_index = std::stoi(tokens[header_map.at("ORIGIN")]);
            int dest_index = std::stoi(tokens[header_map.at("DEST")]);
            time_t departure_time = std::stol(tokens[header_map.at("DEPARTURES_SCHEDULED")]);
            time_t total_time = std::stol(tokens[header_map.at("RAMP_TO_RAMP")]);
            time_t flight_time = std::stol(tokens[header_map.at("AIR_TIME")]);
            double distance = std::stod(tokens[header_map.at("DISTANCE")]);

            std::string origin_name = tokens[header_map.at("ORIGIN_CITY_NAME")];
            std::string dest_name = tokens[header_map.at("DEST_CITY_NAME")];

            airport origin(origin_index, origin_name);
            airport dest(dest_index, dest_name);

            return flight(origin, dest, departure_time, total_time, flight_time, distance);
        }
        catch (const std::exception& e) {
            throw ParseException(line, "������ �������������� ������");
        }
    }

public:
    std::vector<flight> read_flights_from_file(const std::string& filename) {
        std::vector<flight> flights;
        std::ifstream infile(filename);
        if (!infile) {
            throw FileException(filename, "�� ������� ������� ����");
        }

        std::string header_line;
        if (!std::getline(infile, header_line)) {
            throw FileException(filename, "�� ������� ��������� ���������");
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
                std::cerr << "��������������: " << e.what() << std::endl;
            }
        }
        return flights;
    }
};


class flight_graph {
private:
    std::unordered_map<int, std::vector<flight>> adj_list;

public:
    void add_flight(const flight& fl) {
        adj_list[fl.get_origin().get_index()].push_back(fl);
    }

    const std::vector<flight>& get_flights_from(const airport& ap) const {
        auto it = adj_list.find(ap.get_index());
        return (it != adj_list.end()) ? it->second : std::vector<flight>();
    }

    void sort_flights() {
        for (auto& pair : adj_list) {
            std::sort(pair.second.begin(), pair.second.end(),
                [](const flight& a, const flight& b) {
                    return a.get_departure_time() < b.get_departure_time();
                });
        }
    }
};


class shortest_path_finder {
private:
    const flight_graph& graph;

public:
    shortest_path_finder(const flight_graph& g) : graph(g) {}

    std::vector<flight> find_shortest_path(const airport& start, const airport& destination, time_t user_departure_time) {
        std::unordered_map<int, time_t> best_time;
        std::unordered_map<int, flight> prev_flight;
        std::priority_queue<std::pair<time_t, int>, std::vector<std::pair<time_t, int>>, std::greater<std::pair<time_t, int>>> pq;

        best_time[start.get_index()] = user_departure_time;
        pq.push(std::make_pair(user_departure_time, start.get_index()));

        bool found = false;

        while (!pq.empty() && !found) {
            std::pair<time_t, int> current_pair = pq.top();
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
                    pq.push(std::make_pair(arrive, fl.get_destination().get_index()));
                }
            }
        }

        if (!found) {
            throw AlgorithmException("�� ������� ����� ������� �� �������� ���������");
        }

        std::vector<flight> path;
        int curr = destination.get_index();
        while (curr != start.get_index()) {
            if (!prev_flight.count(curr)) {
                throw AlgorithmException("������ �������������� ����");
            }
            path.push_back(prev_flight[curr]);
            curr = prev_flight[curr].get_origin().get_index();
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};


bool get_user_input(airport& start_airport, airport& destination_airport, time_t& user_departure_time) {
    std::cout << "������� ��� ��������� �����������: ";
    std::string start_code;
    if (!std::getline(std::cin, start_code) || start_code.empty()) {
        std::cerr << "�������� ���� ���� ��������� �����������." << std::endl;
        return false;
    }

    std::cout << "������� ��� ��������� ����������: ";
    std::string dest_code;
    if (!std::getline(std::cin, dest_code) || dest_code.empty()) {
        std::cerr << "�������� ���� ���� ��������� ����������." << std::endl;
        return false;
    }

    std::cout << "������� ���� � ����� ������ (����-��-�� ��:��): ";
    std::string date_time_str;
    if (!std::getline(std::cin, date_time_str) || date_time_str.empty()) {
        std::cerr << "�������� ���� ����/�������." << std::endl;
        return false;
    }

    std::istringstream ss(date_time_str);
    std::tm tm_val = {};
    ss >> std::get_time(&tm_val, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        std::cerr << "������: �������� ������ ����/�������." << std::endl;
        return false;
    }
    user_departure_time = mktime(&tm_val);

    start_airport = airport(0, start_code);
    destination_airport = airport(0, dest_code);

    return true;
}


void print_shortest_path(const std::vector<flight>& path, time_t user_departure_time) {
    if (path.empty()) {
        std::cout << "������� �� ������." << std::endl;
        return;
    }

    std::cout << "��������� �������:" << std::endl;
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
        std::cout << "��������� � ����������: ";
        for (size_t i = 0; i < layovers.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << layovers[i];
        }
        std::cout << std::endl;
    }

    std::cout << "����� ����������: " << total_distance << " ����" << std::endl;
    std::cout << "����� ����� � ����: "
        << total_time / 3600 << " ����� "
        << (total_time % 3600) / 60 << " �����." << std::endl;
}


int main() {
    try {
        file_reader reader;
        std::vector<flight> flights = reader.read_flights_from_file("Graph.csv");

        if (flights.empty()) {
            std::cerr << "���� �� �������� ������ � ������." << std::endl;
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
            std::cerr << "������ ����� ������." << std::endl;
            return 1;
        }

        shortest_path_finder spf(graph);
        std::vector<flight> path = spf.find_shortest_path(start, destination, departure_time);

        print_shortest_path(path, departure_time);

    }
    catch (const FileException& e) {
        std::cerr << "������ �����: " << e.what() << std::endl;
        return 1;
    }
    catch (const ParseException& e) {
        std::cerr << "������ ��������: " << e.what() << std::endl;
        return 1;
    }
    catch (const AlgorithmException& e) {
        std::cerr << "������ ���������: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}