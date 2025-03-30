#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <map>

using namespace std;

struct Vertex {
    int prev = -1;
    int dist = numeric_limits<int>::max();
};

// Объявления функций
void loadMappings(const string& filename,
    unordered_map<int, string>& id_to_airport,
    unordered_map<string, int>& airport_to_id);
vector<vector<pair<int, int>>> loadGraph(const string& filename, int max_id);
void findShortestPath(int start, int end,
    const vector<vector<pair<int, int>>>& graph,
    const unordered_map<int, string>& id_to_airport);
int findMaxId(const unordered_map<int, string>& id_to_airport);
void printAirportList(const unordered_map<int, string>& id_to_airport);

// Реализации функций
void loadMappings(const string& filename,
    unordered_map<int, string>& id_to_airport,
    unordered_map<string, int>& airport_to_id) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open mapping file " << filename << endl;
        exit(1);
    }

    string line;
    getline(file, line); // Пропускаем заголовок

    while (getline(file, line)) {
        stringstream ss(line);
        string code, id_str;
        getline(ss, code, ',');
        getline(ss, id_str);
        int id = stoi(id_str);
        id_to_airport[id] = code;
        airport_to_id[code] = id;
    }
}

vector<vector<pair<int, int>>> loadGraph(const string& filename, int max_id) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open graph file " << filename << endl;
        exit(1);
    }

    vector<vector<pair<int, int>>> graph(max_id + 1);
    string line;
    getline(file, line); // Пропускаем заголовок

    while (getline(file, line)) {
        stringstream ss(line);
        string from_str, to_str, dist_str;
        getline(ss, from_str, ',');
        getline(ss, to_str, ',');
        getline(ss, dist_str);

        int from = stoi(from_str);
        int to = stoi(to_str);
        int dist = stoi(dist_str);

        graph[from].emplace_back(to, dist);
    }

    return graph;
}

void findShortestPath(int start, int end,
    const vector<vector<pair<int, int>>>& graph,
    const unordered_map<int, string>& id_to_airport) {
    if (start == end) {
        cout << "Start and end airports are the same.\n";
        return;
    }

    int n = graph.size();
    vector<Vertex> vertices(n);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    vertices[start].dist = 0;
    pq.push({ 0, start });

    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();
        int current_dist = current.first;
        int u = current.second;

        if (current_dist > vertices[u].dist) continue;
        if (u == end) break;

        for (const auto& edge : graph[u]) {
            int v = edge.first;
            int weight = edge.second;
            int new_dist = vertices[u].dist + weight;
            if (new_dist < vertices[v].dist) {
                vertices[v].dist = new_dist;
                vertices[v].prev = u;
                pq.push({ new_dist, v });
            }
        }
    }

    if (vertices[end].dist == numeric_limits<int>::max()) {
        cout << "No flight path exists between " << id_to_airport.at(start)
            << " and " << id_to_airport.at(end) << endl;
    }
    else {
        cout << "\nShortest flight path:\n";
        vector<int> path;
        for (int v = end; v != -1; v = vertices[v].prev) {
            path.push_back(v);
        }

        cout << "Route: ";
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            cout << id_to_airport.at(*it);
            if (it + 1 != path.rend()) cout << " -> ";
        }
        cout << "\nTotal distance: " << vertices[end].dist << " miles\n";
    }
}

int findMaxId(const unordered_map<int, string>& id_to_airport) {
    int max_id = 0;
    for (const auto& pair : id_to_airport) {
        if (pair.first > max_id) {
            max_id = pair.first;
        }
    }
    return max_id;
}

void printAirportList(const unordered_map<int, string>& id_to_airport) {
    cout << "\nAvailable airports (" << id_to_airport.size() << "):\n";
    cout << "----------------------------------------\n";
    cout << left << setw(10) << "ID" << setw(10) << "Code" << "\n";
    cout << "----------------------------------------\n";

    // Создаем временный map для сортировки по ID
    map<int, string> sorted_airports(id_to_airport.begin(), id_to_airport.end());

    for (const auto& pair : sorted_airports) {
        cout << setw(10) << pair.first << setw(10) << pair.second << "\n";
    }

    cout << "----------------------------------------\n";
    cout << "Example: For JFK -> LAX, enter 'JFK' then 'LAX'\n";
}

int main() {
    cout << "=== Airline Route Finder ===\n";

    // Загрузка данных
    unordered_map<int, string> id_to_airport;
    unordered_map<string, int> airport_to_id;
    loadMappings("airport_mapping.csv", id_to_airport, airport_to_id);

    int max_id = findMaxId(id_to_airport);
    auto graph = loadGraph("airline_graph.csv", max_id);

    // Вывод списка аэропортов
    printAirportList(id_to_airport);

    // Основной цикл программы
    while (true) {
        cout << "\nEnter departure airport code (3 letters, e.g. LAX): ";
        string start_code;
        cin >> start_code;

        cout << "Enter arrival airport code (3 letters, e.g. JFK): ";
        string end_code;
        cin >> end_code;

        // Преобразование в верхний регистр
        transform(start_code.begin(), start_code.end(), start_code.begin(), ::toupper);
        transform(end_code.begin(), end_code.end(), end_code.begin(), ::toupper);

        if (airport_to_id.count(start_code) && airport_to_id.count(end_code)) {
            findShortestPath(airport_to_id[start_code], airport_to_id[end_code], graph, id_to_airport);
        }
        else {
            cout << "Invalid airport code! Please try again.\n";
            continue;
        }

        cout << "\nAnother search? (y/n): ";
        char choice;
        cin >> choice;
        if (tolower(choice) != 'y') break;
    }

    return 0;
}
