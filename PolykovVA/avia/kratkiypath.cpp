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


using namespace std;


//Хранение информации о маршруте
struct Route {
    string destination;//принимающий аэропорт
    int airtime;//время полета
};


unordered_map<string, vector<Route>> flight_graph;


void load_graph(const string& file_path) {
    flight_graph.clear();

    ifstream input_file(file_path);
    if (!input_file.is_open()) {
        throw runtime_error("Error: Failed to open the file: " + file_path);
    }


    string header_line;
    getline(input_file, header_line);//пропуск


    string data_line;
    while (getline(input_file, data_line)) {
        stringstream ss(data_line);
        string origin, dest, airtime_str;


        // код аэропорта отправления, назначения, время полета
        getline(ss, origin, ',');
        getline(ss, dest, ',');
        getline(ss, airtime_str);


        try {
            flight_graph[origin].push_back({ dest, stoi(airtime_str) });//добавляем маршрут
            flight_graph[dest];
        }


        catch (const exception& e) {
            cerr << "Warning: Invalid data in string: " << data_line << " - " << e.what() << endl;
            continue;
        }
    }
}


// Дейкстра
void find_shortest_path(const string& start_airport, const string& end_airport) {
    //  минимальное время 
    unordered_map<string, int> total_time;
    // предыдущий аэропорт 
    unordered_map<string, string> previous;
    // Приоритетная очередь
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> pq;


    // - бесконечное время 
    for (const auto& [airport, _] : flight_graph) {
        total_time[airport] = numeric_limits<int>::max();
    }
    //старт
    total_time[start_airport] = 0;
    pq.push({ 0, start_airport });


    while (!pq.empty()) {
        auto [current_time, current_airport] = pq.top();
        pq.pop();

        //конечный
        if (current_airport == end_airport) break;

        //более быстрый
        if (current_time > total_time[current_airport]) continue;


        //для смежного рейса
        for (const Route& route : flight_graph[current_airport]) {
            int new_time = current_time + route.airtime;
            // Если нашли более короткий путь до соседнего аэропорта
            if (new_time < total_time[route.destination]) {
                total_time[route.destination] = new_time;
                previous[route.destination] = current_airport;
                pq.push({ new_time, route.destination });
            }
        }
    }


    //путь
    if (total_time[end_airport] == numeric_limits<int>::max()) {
        throw runtime_error("There is no path between " + start_airport + " and " + end_airport);
    }


    // Восстанавление пути
    vector<string> path;
    for (string airport = end_airport; !airport.empty(); airport = previous[airport]) {
        path.push_back(airport);
    }
    reverse(path.begin(), path.end());


    //вывод
    cout << "\nShortest route (" << total_time[end_airport] << " minutes):\n";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i != path.size() - 1) cout << " -> ";
    }
    cout << endl;
}


void print_available_airports() {
    cout << "\nAvailable airports in the database:\n";
    int count = 0;
    for (const auto& [airport, _] : flight_graph) {
        cout << airport << " ";
        if (++count % 10 == 0) cout << endl;
    }
    cout << "\nAll: " << count << " airports\n";
}


int main() {
    try {
        load_graph("filtered_data.csv");

        print_available_airports();

        string start, end;
        cout << "\nEnter the airport code of departure: ";
        cin >> start;
        cout << "Enter the destination airport code: ";
        cin >> end;

        transform(start.begin(), start.end(), start.begin(), ::toupper);
        transform(end.begin(), end.end(), end.begin(), ::toupper);

        if (!flight_graph.count(start)) {
            throw runtime_error("Departure airport '" + start + "' not found in the database");
        }
        if (!flight_graph.count(end)) {
            throw runtime_error("Destination airport '" + end + "' not found in the database");
        }

        find_shortest_path(start, end);
        return 0;

    }
    catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }
}