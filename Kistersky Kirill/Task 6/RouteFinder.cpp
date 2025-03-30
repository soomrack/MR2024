#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct AirportInfo {
    string code;
    string city;
};

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

string getInputFilename() {
    string filename;
    while (true) {
        cout << "Enter input CSV filename (or drag file here): ";

        if (cin.peek() == '\n') {
            clearInputBuffer();
        }
        getline(cin, filename);

        // Удаляем кавычки если файл перетащен в окно терминала
        filename.erase(remove(filename.begin(), filename.end(), '"'), filename.end());

        // Удаляем возможные пробелы в начале и конце
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);

        if (filename.empty()) {
            cout << "Filename cannot be empty. Please try again.\n";
            continue;
        }

        if (!fileExists(filename)) {
            cout << "File '" << filename << "' not found. Please try again.\n";
            continue;
        }

        return filename;
    }
}

void convertAirlineData(const string& inputFile, const string& outputFile, const string& mappingFile) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);
    ofstream mapFile(mappingFile);

    if (!inFile.is_open()) {
        cerr << "Error: Could not open input file " << inputFile << endl;
        return;
    }
    if (!outFile.is_open()) {
        cerr << "Error: Could not create output file " << outputFile << endl;
        return;
    }
    if (!mapFile.is_open()) {
        cerr << "Error: Could not create mapping file " << mappingFile << endl;
        return;
    }

    // Заголовки выходных файлов
    outFile << "from,to,distance\n";
    mapFile << "original_code,normalized_id,city\n";  // Добавили город

    unordered_map<string, pair<int, string>> airportCodes;  // code -> (id, city)
    int currentId = 1;
    string line;
    int processedCount = 0;
    int errorCount = 0;

    // Читаем заголовок входного файла
    getline(inFile, line);
    vector<string> headers;
    stringstream headerStream(line);
    string header;

    while (getline(headerStream, header, ',')) {
        headers.push_back(header);
    }

    // Находим индексы нужных столбцов
    int originIdx = -1, destIdx = -1, distIdx = -1, originCityIdx = -1, destCityIdx = -1;
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == "ORIGIN") originIdx = i;
        else if (headers[i] == "DEST") destIdx = i;
        else if (headers[i] == "DISTANCE") distIdx = i;
        else if (headers[i] == "ORIGIN_CITY_NAME") originCityIdx = i;
        else if (headers[i] == "DEST_CITY_NAME") destCityIdx = i;
    }

    if (originIdx == -1 || destIdx == -1 || distIdx == -1) {
        cerr << "Error: Required columns not found in input file" << endl;
        return;
    }

    while (getline(inFile, line)) {
        try {
            stringstream ss(line);
            vector<string> row;
            string cell;

            // Читаем строку с учетом кавычек
            bool inQuotes = false;
            string currentCell;
            for (char c : line) {
                if (c == '"') {
                    inQuotes = !inQuotes;
                }
                else if (c == ',' && !inQuotes) {
                    row.push_back(currentCell);
                    currentCell.clear();
                }
                else {
                    currentCell += c;
                }
            }
            row.push_back(currentCell);

            // Проверяем что строка содержит достаточно данных
            if (row.size() <= max(originIdx, max(destIdx, distIdx))) {
                errorCount++;
                continue;
            }

            string origin = row[originIdx];
            string dest = row[destIdx];
            string distStr = row[distIdx];
            string originCity = originCityIdx != -1 ? row[originCityIdx] : "Unknown";
            string destCity = destCityIdx != -1 ? row[destCityIdx] : "Unknown";

            // Удаляем возможные оставшиеся кавычки
            origin.erase(remove(origin.begin(), origin.end(), '"'), origin.end());
            dest.erase(remove(dest.begin(), dest.end(), '"'), dest.end());
            distStr.erase(remove(distStr.begin(), distStr.end(), '"'), distStr.end());

            // Пропускаем пустые значения
            if (origin.empty() || dest.empty() || distStr.empty()) {
                errorCount++;
                continue;
            }

            int distance = stoi(distStr);

            // Добавляем аэропорты в маппинг если их еще нет
            if (airportCodes.find(origin) == airportCodes.end()) {
                airportCodes[origin] = { currentId, originCity };
                mapFile << origin << "," << currentId << "," << originCity << "\n";
                currentId++;
            }
            if (airportCodes.find(dest) == airportCodes.end()) {
                airportCodes[dest] = { currentId, destCity };
                mapFile << dest << "," << currentId << "," << destCity << "\n";
                currentId++;
            }

            // Записываем ребро графа
            outFile << airportCodes[origin].first << ","
                << airportCodes[dest].first << ","
                << distance << "\n";

            processedCount++;
            if (processedCount % 10000 == 0) {
                cout << "Processed " << processedCount << " records..." << endl;
            }

        }
        catch (...) {
            errorCount++;
            continue;
        }
    }

    cout << "\nConversion complete!\n"
        << "- Output graph: " << outputFile << "\n"
        << "- Airport mapping: " << mappingFile << "\n"
        << "- Total airports: " << airportCodes.size() << "\n"
        << "- Total routes processed: " << processedCount << "\n"
        << "- Errors encountered: " << errorCount << endl;
}

int main() {
    cout << "=== Airline Data Converter ===" << endl;

    string inputFile = getInputFilename();
    string outputFile = "airline_graph.csv";
    string mappingFile = "airport_mapping.csv";

    convertAirlineData(inputFile, outputFile, mappingFile);

    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}#include <iostream>
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
