#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue> 
#include <climits> 
#include <algorithm>
#include <stdexcept>
#include <string>

using namespace std;

// Структура для хранения информации о рейсе
struct Flight {
    string destination;  
    int time;  
};


// Разделяет строку на части по запятым
vector<string> split_csv_line(string line) {
    vector<std::string> cols;
    string col;
    bool in_quotes = false;  //Флаг указывает находимся ли мы внутри кавычек
   
    string processed_line = line;
    if (!line.empty() && line.front() == '"' && line.back() == '"') {
        processed_line = line.substr(1, line.size() - 2);
    }
    for (char c : processed_line) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            cols.push_back(col);  //Добавить в конец
            col.clear();  //Очистка временной строки
        } else {
            col += c;
        }
    }
    if (!col.empty())
    cols.push_back(col);
    
    
    return cols;
}


// Обрабатать данные о рейсах
void extract_csv_data(string in_file, string out_file) {
    // Открыть файлы
    ifstream in(in_file);
    ofstream out(out_file);
    
    if (!in) throw runtime_error("Cannot open file");
    if (!out) throw runtime_error("Cannot open out file");

    // Читать заголовок
    string header;
    getline(in, header);
    
    // Находить номера нужных столбцов
    vector<string> cols = split_csv_line(header); //разбитие первой строки
    int from_col = -1, to_col = -1, id_col = -1, time_col = -1; 
    
    for (int i = 0; i < cols.size(); i++) {
        if (cols[i] == "ORIGIN") from_col = i;
        if (cols[i] == "DEST") to_col = i;
        if (cols[i] == "AIRLINE_ID") id_col = i;
        if (cols[i] == "AIR_TIME") time_col = i;  
    }
    
    if (from_col == -1 || to_col == -1 ||  id_col == -1 ||  time_col == -1) {
        throw runtime_error("Required columns not found");
    } //нет столбца = ошибка

    // Считаем сколько раз встречается каждый маршрут
    unordered_map<string, int> route_counts;
    string line;
    
    while (getline(in, line)) {
        vector<string> data = split_csv_line(line);
        if (data.size() <= max(from_col, max(to_col, time_col))) continue;
        
        string route = data[from_col] + "->" + data[to_col];
        route_counts[route]++;
    }

    // Маршруты
    in.clear();  //убирает элементы
    in.seekg(0);  // Возвращает указатель чтения в начало файла
    getline(in, header);  // Пропускаем заголовок
    
    out << "ORIGIN,DEST,ID,AIR_TIME\n"; // Новый заголовок
    
    while (getline(in, line)) {
        vector<string> data = split_csv_line(line);
        // Проверяем, что строка содержит все нужные столбцы
        if (data.size() <= max({from_col, to_col, time_col, id_col})) continue;
        
        string route = data[from_col] + "->" + data[to_col];
        string num = data[id_col];
        string time = data[time_col];
        
        // Удаляем кавычки
        time.erase(remove(time.begin(), time.end(), '"'), time.end());
        
        try {
            int minutes = stoi(time); // Преобразует строку в число
            if (minutes > 0 && route_counts[route] >= 20) {
                out << data[from_col] << "," << data[to_col] << "," << num << "," << minutes << "\n";
            }
        } 
        catch (const invalid_argument& e) // функции передан некорректный аргумент, не соответствующий условиям
        {
        throw runtime_error("Error parsing data at line");
        }
    }
}

// Реализация алгоритма Дейкстры для поиска кратчайших путей
void shortest_paths(const string& result_file) {
    // Создать граф рейсов из обработанных данных
    unordered_map<string, vector<Flight>> flight_graph;
    
    ifstream in(result_file);
    string line;
    getline(in, line); // Пропуск заголовка
    
    while (getline(in, line)) {
       vector<string> data = split_csv_line(line);
        if (data.size() < 4) continue;
        
        string from = data[0];
        string to = data[1];
        int time = stoi(data[3]);
        
        flight_graph[from].push_back(Flight{to, time});
    }

   // Ввод аэропорта отправления
    string start;
    cout << "Enter departure airport: ";
    cin >> start;
    
    // Проверка существования аэропорта отправления
    if (flight_graph.find(start) == flight_graph.end()) {
        cout << "Departure airport not found!\n";
        return;
    }

    // Ввод аэропорта прибытия
    string finish;
    int attempts = 3;
    
       
    while (attempts > 0) {
        cout << "Enter destination airport (" << attempts << " attempts left): ";
        cin >> finish;
        
        if (flight_graph.find(finish) != flight_graph.end()) {
            break; // Аэропорт найден
        }
        
        cout << "Airport not found ";
        attempts--;
        

        if (attempts == 0) {
            cout << "Available destinations: ";
            for (const auto& name : flight_graph) {
                if (name.first != start) {
                    cout << name.first << " ";
                }
            }
            cout << endl;
            return;
        }
    }

   // Алгоритм Дейкстры
    unordered_map<string, int> min_t;
    unordered_map<string, string> prev;   // Хранит предыдущий аэропорт
    
    for (const auto& pair : flight_graph) {
        min_t[pair.first] = INT_MAX;
    }
    min_t[start] = 0;
    
    //упрядочить элементы
    priority_queue<pair<int, string>> pq;
    pq.push(make_pair(0, start));

    while (!pq.empty()) {
        int time = -pq.top().first;
        string airport = pq.top().second;
        pq.pop(); 
        
        if (airport == finish) break;
        if (time > min_t[airport]) continue;
        
        for (const Flight& flight : flight_graph[airport]) {
            int new_time = min_t[airport] + flight.time;
            
            if (new_time < min_t[flight.destination]) {
                min_t[flight.destination] = new_time;
                prev[flight.destination] = airport;
                pq.push(make_pair(-new_time, flight.destination));
            }
        }
    }

    // Вывод результатов
    if (min_t[finish] != INT_MAX) {
        cout << "\nFastest route from " << start << " to " << finish << ":\n";
        cout << "Total time: " << min_t[finish] << " min\n";
        cout << "Route: ";
        
        vector<string> path;
        for (string at = finish; at != start; at = prev[at]) {
            path.push_back(at);
        }
        
        cout << start;
        for (int i = path.size() - 1; i >= 0; --i) {
            cout << " -> " << path[i];
        }
        cout << endl;
    } else {
        cout << "\nNo available route from " << start << " to " << finish << endl;
    }
}

int main() {
   try {
        // Обрабатываем исходные данные
        cout << "Processing data..." << endl;
        extract_csv_data("Airport.csv", "result.csv");
        cout << "Successfully created file: result.csv" << endl;
        
        // Запускаем поиск кратчайших путей
        shortest_paths("result.csv");
        
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}