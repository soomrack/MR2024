#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <iterator>

using namespace std;

struct Destinations {
    string destination;
    int air_time;
    int distance;
};

struct Airport {
    string city_name;
    vector<Destinations> destinations;
};


vector<string> split(const string& line)
{
    vector<string> splitted_line;
    string word;
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

// получение данных из CSV и запись в граф
unordered_map<string, Airport> get_airport_data(const string& file_name)
{
    ifstream file(file_name);

    // Порядковые номера нужных позиций из CSV
    size_t distance_file_number = 7;
    size_t air_time_file_number = 9;
    size_t origin_file_number = 22;
    size_t origin_city_name_file_number = 23;
    size_t destination_file_number = 33;
    size_t destination_city_name_file_number = 34;
    
    unordered_map<string, Airport> airports;
    
    if (!file.is_open()) {
        cerr << "File was not opened";
        throw invalid_argument("File was not opened");
    }
    
    string line;
    while (getline(file, line)) {
        vector<string> words = split(line);

        int air_time = 0;
        int distance = 0;
        if (words[distance_file_number] != "DISTANCE") {
            try {
                if (stoi(words[air_time_file_number]) == 0) {
                    air_time = numeric_limits<int>::max();
                }
                else {
                    air_time = stoi(words[air_time_file_number]);
                }
                distance = stoi(words[distance_file_number]);
            }
            catch (...) {
                cerr << "Invalid air time: " << words[air_time_file_number] << "or ";
                cerr << "Invalid distance: " << words[distance_file_number];
                continue;
            }
            string origin = words[origin_file_number];
            string origin_city_name = words[origin_city_name_file_number];
            string destination = words[destination_file_number];
            string destination_city_name = words[destination_city_name_file_number];

            // Добавление аэропорта отправления
            if (airports.find(origin) == airports.end()) {
                airports[origin] = { origin_city_name, {} };
            }

            // Добавление аэропорта прибытия
            if (airports.find(destination) == airports.end()) {
                airports[destination] = { destination_city_name, {} };
            }

            // Добавление в граф
            airports[origin].destinations.push_back({ destination, air_time, distance });
        }
    }
    
    file.close();
    return airports;
}

// Поиск кратчайшего пути (алгоритм Дейкстры)
tuple<vector<string>, int, int> find_shortest_path(const unordered_map<string, Airport>& airports,
    const string& start, const string& end)
{
    if (airports.find(start) == airports.end() || airports.find(end) == airports.end()) {
        return {{}, -1, {}};
    }
    
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> queue;
    unordered_map<string, int> time_to_fly;
    unordered_map<string, int> dist_to_fly;
    unordered_map<string, string> order;
    
    for (const auto& entry : airports) {
        time_to_fly[entry.first] = numeric_limits<int>::max();
    }
    
    time_to_fly[start] = 0;
    dist_to_fly[start] = 0;
    queue.push({0, start});
    
    while (!queue.empty()) {
        string current_airport = queue.top().second;
        int current_time = queue.top().first;
        queue.pop();

        if (current_airport == end) break;
        
        for (const Destinations& destination : airports.at(current_airport).destinations) {
            string checking_airport = destination.destination;
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
    if (time_to_fly[end] == numeric_limits<int>::max()) return {{}, - 1, {}};
    
    vector<string> path;
    for (string at = end; at != ""; at = order[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {path, time_to_fly[end], dist_to_fly[end]};
}

void request(const string& file_name) {
    auto airports = get_airport_data(file_name);

    string start_airport;
    string finish_airport;
    cout << "Enter start airport code: ";
    cin >> start_airport;
    cout << "Enter finish airport code: ";
    cin >> finish_airport;
    
    auto shortest_path = find_shortest_path(airports, start_airport, finish_airport);
    auto& path = get<0>(shortest_path);
    int total_time = get<1>(shortest_path);
    int total_distance = get<2>(shortest_path);

    if (path.empty() || total_time == -1) {
        cout << "No path found" << endl;
    }
    else {
        cout << "Optimal path:" << endl;
        for (size_t idx = 0; idx < path.size(); idx++) {
            if (idx != 0) cout << " - ";
            cout << airports[path[idx]].city_name;
        }
        cout << "\ntotal time: " << total_time << " minutes" << endl;
        cout << "total distance: " << total_distance << " miles" << endl;
    }
}

int main() {
    request("T_T100_SEGMENT_ALL_CARRIER.csv");
    
    return 1;
}