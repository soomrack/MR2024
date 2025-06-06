#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <iterator>

#define airport_id std::string

struct Destinations {
    airport_id destination;
    int air_time;
    int distance;
};

struct Airport {
    std::string city_name;
    std::vector<Destinations> destinations;
};


std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> splitted_line;
    std::string word;
    bool in_quotes = false;
    
    for (char letter : line) {
        if (letter == '"') {
            in_quotes = !in_quotes;
        }
        else if (letter == ',' && !in_quotes) {
            splitted_line.push_back(word);
            word.clear();
        }
        else {
            word += letter;
        }
    }
    splitted_line.push_back(word);
    
    return splitted_line;
}

// ��������� ������ �� CSV � ������ � ����
std::unordered_map<airport_id, Airport> get_airport_data(const std::string& file_name)
{
    std::ifstream file(file_name);

    // ���������� ������ ������ ������� �� CSV
    size_t distance_file_number = 7;
    size_t air_time_file_number = 9;
    size_t origin_file_number = 22;
    size_t origin_city_name_file_number = 23;
    size_t destination_file_number = 33;
    size_t destination_city_name_file_number = 34;
    
    std::unordered_map<airport_id, Airport> airports;
    
    if (!file.is_open()) {
        std::cerr << "File was not opened";
        throw std::invalid_argument("File was not opened");
    }
    
    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> words = split(line);

        int air_time = 0;
        int distance = 0;
        if (words[distance_file_number] != "DISTANCE") {
            try {
                if (stoi(words[air_time_file_number]) == 0) {
                    air_time = std::numeric_limits<int>::max();
                }
                else {
                    air_time = stoi(words[air_time_file_number]);
                }
                distance = stoi(words[distance_file_number]);
            }
            catch (...) {
                std::cerr << "Invalid air time: " << words[air_time_file_number] << "or ";
                std::cerr << "Invalid distance: " << words[distance_file_number];
                continue;
            }
            std::string origin = words[origin_file_number];
            std::string origin_city_name = words[origin_city_name_file_number];
            std::string destination = words[destination_file_number];
            std::string destination_city_name = words[destination_city_name_file_number];

            // ���������� ��������� �����������
            if (airports.find(origin) == airports.end()) {
                airports[origin] = { origin_city_name, {} };
            }

            // ���������� ��������� ��������
            if (airports.find(destination) == airports.end()) {
                airports[destination] = { destination_city_name, {} };
            }

            // ���������� � ����
            airports[origin].destinations.push_back({ destination, air_time, distance });
        }
    }
    
    file.close();
    return airports;
}

// ����� ����������� ���� (�������� ��������)
std::tuple<std::vector<airport_id>, int, int> find_shortest_path(const std::unordered_map<airport_id, Airport>& airports,
    const airport_id& start, const airport_id& end)
{
    if (airports.find(start) == airports.end() || airports.find(end) == airports.end()) {
        return {{}, -1, {}};
    }
    
    std::priority_queue<std::pair<int, airport_id>, std::vector<std::pair<int, airport_id>>, std::greater<std::pair<int, airport_id>>> queue;
    std::unordered_map<airport_id, int> time_to_fly;
    std::unordered_map<airport_id, int> dist_to_fly;
    std::unordered_map<airport_id, airport_id> order;
    
    for (const auto& entry : airports) {
        time_to_fly[entry.first] = std::numeric_limits<int>::max();
    }
    
    time_to_fly[start] = 0;
    dist_to_fly[start] = 0;
    queue.push({0, start});
    
    while (!queue.empty()) {
        airport_id current_airport = queue.top().second;
        int current_time = queue.top().first;
        queue.pop();

        if (current_airport == end) break;
        
        for (const Destinations& destination : airports.at(current_airport).destinations) {
            airport_id checking_airport = destination.destination;
            int checking_time = time_to_fly[current_airport] + destination.air_time;
            int checking_dist = dist_to_fly[current_airport] + destination.distance;

            if (checking_time < time_to_fly[checking_airport]) {
                time_to_fly[checking_airport] = checking_time;
                dist_to_fly[checking_airport] = checking_dist;
                order[checking_airport] = current_airport;
                queue.push({checking_time, checking_airport});
            }
        }
    }
    if (time_to_fly[end] == std::numeric_limits<int>::max()) return {{}, - 1, {}};
    
    std::vector<airport_id> path;
    for (std::string at = end; at != ""; at = order[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {path, time_to_fly[end], dist_to_fly[end]};
}

void request(const std::string& file_name) {
    auto airports = get_airport_data(file_name);

    airport_id start_airport;
    airport_id finish_airport;
    std::cout << "Enter start airport code: ";
    std::cin >> start_airport;
    std::cout << "Enter finish airport code: ";
    std::cin >> finish_airport;
    
    auto shortest_path = find_shortest_path(airports, start_airport, finish_airport);
    auto& path = std::get<0>(shortest_path);
    int total_time = std::get<1>(shortest_path);
    int total_distance = std::get<2>(shortest_path);

    if (path.empty() || total_time == -1) {
        std::cout << "No path found" << std::endl;
    }
    else {
        std::cout << "Optimal path:" << std::endl;
        for (size_t idx = 0; idx < path.size(); idx++) {
            if (idx != 0) std::cout << " - ";
            std::cout << airports[path[idx]].city_name;
        }
        std::cout << "\ntotal time: " << total_time << " minutes" << std::endl;
        std::cout << "total distance: " << total_distance << " miles" << std::endl;
    }
}

int main() {
    request("T_T100_SEGMENT_ALL_CARRIER.csv");
    
    return 1;
}