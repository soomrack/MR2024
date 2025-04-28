#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <climits>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

map<string, int> cityIndex;
vector<vector<pair<int, int>>> graph;

void addFlight(const string &from, const string &to, int cost);
int dijkstra(int start, int end);
void loadFlightsFromCSV(const string& filename);

void addFlight(const string &from, const string &to, int cost) {
    if (cityIndex.find(from) == cityIndex.end()) {
        cityIndex[from] = cityIndex.size();
        graph.resize(cityIndex.size());
    }
    if (cityIndex.find(to) == cityIndex.end()) {
        cityIndex[to] = cityIndex.size();
        graph.resize(cityIndex.size());
    }
    graph[cityIndex[from]].push_back({cityIndex[to], cost});
}

void loadFlightsFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string origin, dest, rampTimeStr;

        getline(ss, origin, ',');
        getline(ss, rampTimeStr, ',');
        getline(ss, dest, ',');

        try {
            int rampTime = stoi(rampTimeStr);
            addFlight(origin, dest, rampTime);
        } catch (...) {
            cerr << "Warning: Invalid data in line: " << line << endl;
        }
    }
}

int dijkstra(int start, int end) {
    int V = graph.size();
    vector<int> dist(V, INT_MAX);
    dist[start] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int currentDist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (u == end) {
            return dist[end];
        }

        if (currentDist > dist[u]) continue;

        for (auto &[v, weight] : graph[u]) {
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    return dist[end];
}

int main() {
    loadFlightsFromCSV("filtred_flights.csv");

    string city_from, city_to;
    cout << "Enter departure city code (e.g. JFK): ";
    getline(cin, city_from);
    cout << "Enter destination city code (e.g. LAX): ";
    getline(cin, city_to);

    if (cityIndex.find(city_from) == cityIndex.end() || cityIndex.find(city_to) == cityIndex.end()) {
        cerr << "Error: One or both cities not found in database!" << endl;
        return 1;
    }

    int distance = dijkstra(cityIndex[city_from], cityIndex[city_to]);

    if (distance == INT_MAX) {
        cout << "No flight path exists between " << city_from << " and " << city_to << "!" << endl;
    } else {
        cout << "The shortest flight time from " << city_from << " to " << city_to 
             << " is: " << distance << " minutes" << endl;
    }

    return 0;
}
