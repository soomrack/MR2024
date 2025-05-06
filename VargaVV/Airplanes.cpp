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
    string destination; 
    int airtime;        
};

unordered_map<string, vector<Route>> flight_graph;

void load_graph(const string& file_path) {
    flight_graph.clear();
    
    ifstream input_file(file_path);
    if (!input_file.is_open()) {
        throw runtime_error("Ошибка: Не удалось открыть файл: " + file_path);
    }

  
    string header_line;
    getline(input_file, header_line);

    string data_line;
    while (getline(input_file, data_line)) {
        stringstream ss(data_line);
        string origin, dest, airtime_str;
        
        // код аэропорта отправления, назначения, время полета
        getline(ss, origin, ',');
        getline(ss, dest, ',');
        getline(ss, airtime_str);

        try {
            flight_graph[origin].push_back({dest, stoi(airtime_str)});
            flight_graph[dest]; 
        } catch (const exception& e) {
            cerr << "Предупреждение: Некорректные данные в строке: " << data_line << " - " << e.what() << endl;
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

    // - бесконечность
    for (const auto& [airport, _] : flight_graph) {
        total_time[airport] = numeric_limits<int>::max();
    }
    //старт
    total_time[start_airport] = 0;
    pq.push({0, start_airport});

    while (!pq.empty()) {
        auto [current_time, current_airport] = pq.top();
        pq.pop();

        if (current_airport == end_airport) break;

        if (current_time > total_time[current_airport]) continue;


        for (const Route& route : flight_graph[current_airport]) {
            int new_time = current_time + route.airtime;
            // Если нашли более короткий путь до соседнего аэропорта
            if (new_time < total_time[route.destination]) {
                total_time[route.destination] = new_time;
                previous[route.destination] = current_airport;
                pq.push({new_time, route.destination});
            }
        }
    }

    if (total_time[end_airport] == numeric_limits<int>::max()) {
        throw runtime_error("Не существует пути между " + start_airport + " и " + end_airport);
    }

    // Восстанавление пути
    vector<string> path;
    for (string airport = end_airport; !airport.empty(); airport = previous[airport]) {
        path.push_back(airport);
    }
    reverse(path.begin(), path.end());

    cout << "\nКратчайший путь (" << total_time[end_airport] << " минут):\n";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i != path.size() - 1) cout << " -> ";
    }
    cout << endl;
}


void print_available_airports() {
    cout << "\nДоступные аэропорты в базе данных:\n";
    int count = 0;
    for (const auto& [airport, _] : flight_graph) {
        cout << airport << " ";
        if (++count % 10 == 0) cout << endl;
    }
    cout << "\nВсего: " << count << " аэропортов\n";
}

int main() {
    try {
        load_graph("filtered_data.csv");
        
        print_available_airports();
        
        string start, end;
        cout << "\nВведите код аэропорта отправления (например, LAS): ";
        cin >> start;
        cout << "Введите код аэропорта назначения (например, LAX): ";
        cin >> end;

        transform(start.begin(), start.end(), start.begin(), ::toupper);
        transform(end.begin(), end.end(), end.begin(), ::toupper);

        if (!flight_graph.count(start)) {
            throw runtime_error("Аэропорт отправления '" + start + "' не найден в базе данных");
        }
        if (!flight_graph.count(end)) {
            throw runtime_error("Аэропорт назначения '" + end + "' не найден в базе данных");
        }

        find_shortest_path(start, end);
        return 0;

    } catch (const exception& e) {
        cerr << "\nОшибка: " << e.what() << endl;
        return 1;
    }
}
