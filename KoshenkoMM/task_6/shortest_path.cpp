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
#include <iomanip>

using namespace std;

struct Route {
    string destination;
    int airtime;
    string flight_id;  // Добавлен идентификатор рейса
};

unordered_map<string, vector<Route>> flight_graph;

void load_graph(const string& file_path) {
    flight_graph.clear();
    ifstream file(file_path);
    
    if (!file) throw runtime_error("Cannot open file: " + file_path);

    string line;
    getline(file, line); // Пропуск заголовка

    while (getline(file, line)) {
        stringstream ss(line);
        string origin, dest, airtime_str;
        
        getline(ss, origin, ',');
        getline(ss, dest, ',');
        getline(ss, airtime_str);

        try {
            flight_graph[origin].push_back({dest, stoi(airtime_str), "FL-" + origin + "-" + dest});
        } catch (...) {
            continue;
        }
    }
}

void print_shortest_path(const string& start, const string& end) {
    // Инициализация
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    unordered_map<string, string> flight_used;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> pq;

    for (const auto& [airport, _] : flight_graph) {
        dist[airport] = numeric_limits<int>::max();
    }
    dist[start] = 0;
    pq.push({0, start});

    // Алгоритм Дейкстры
    while (!pq.empty()) {
        auto [current_dist, current] = pq.top();
        pq.pop();

        if (current == end) break;
        if (current_dist > dist[current]) continue;

        for (const Route& route : flight_graph[current]) {
            int new_dist = current_dist + route.airtime;
            if (new_dist < dist[route.destination]) {
                dist[route.destination] = new_dist;
                prev[route.destination] = current;
                flight_used[route.destination] = route.flight_id;
                pq.push({new_dist, route.destination});
            }
        }
    }

    // Проверка наличия пути
    if (dist[end] == numeric_limits<int>::max()) {
        throw runtime_error("No path exists between " + start + " and " + end);
    }

    // Восстановление пути
    vector<string> path;
    vector<string> flights;
    for (string airport = end; !airport.empty(); airport = prev[airport]) {
        path.push_back(airport);
        if (prev.count(airport)) {
            flights.push_back(flight_used[airport]);
        }
    }
    reverse(path.begin(), path.end());
    reverse(flights.begin(), flights.end());

    // Вывод результата
    cout << "\nShortest path from " << start << " to " << end << " (" 
         << dist[end] << " minutes):\n\n";
    
    cout << left << setw(8) << "From" << setw(8) << "To" 
         << setw(12) << "Flight" << "Duration\n";
    cout << string(35, '-') << "\n";

    for (size_t i = 0; i < path.size()-1; ++i) {
        cout << setw(8) << path[i] 
             << setw(8) << path[i+1]
             << setw(12) << flights[i]
             << flight_graph[path[i]][i].airtime << " min\n";
    }

    cout << "\nTotal journey time: " << dist[end] << " minutes\n";
}

int main(int argc, char* argv[]) {
    try {
        string file_path = "filtered_file.csv";
        if (argc > 1) file_path = argv[1];

        load_graph(file_path);

        string start, end;
        cout << "Enter origin airport code: ";
        cin >> start;
        cout << "Enter destination airport code: ";
        cin >> end;

        transform(start.begin(), start.end(), start.begin(), ::toupper);
        transform(end.begin(), end.end(), end.begin(), ::toupper);

        if (!flight_graph.count(start)) throw runtime_error("Invalid origin airport");
        if (!flight_graph.count(end)) throw runtime_error("Invalid destination airport");

        print_shortest_path(start, end);
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}