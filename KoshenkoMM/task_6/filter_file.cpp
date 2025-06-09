#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>

using namespace std;

vector<string> parse_csv_line(const string& line) {
    vector<string> columns;
    string current_column;
    bool in_quotes = false;

    for (char c : line) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            columns.push_back(current_column);
            current_column.clear();
        } else {
            current_column += c;
        }
    }
    columns.push_back(current_column);
    return columns;
}

void clean_quotes(string& str) {
    str.erase(remove(str.begin(), str.end(), '"'), str.end());
}

void filter_csv(const string& input_path, const string& output_path, int min_frequency = 15) {
    ifstream input_file(input_path);
    ofstream output_file(output_path);
    
    // Проверка файлов
    if (!input_file) throw runtime_error("Cannot open input file: " + input_path);
    if (!output_file) throw runtime_error("Cannot create output file: " + output_path);

    // Обработка заголовка
    string header_line;
    if (!getline(input_file, header_line)) {
        throw runtime_error("Input file is empty");
    }
    
    auto headers = parse_csv_line(header_line);
    
    // Поиск нужных колонок
    struct { int origin, dest, air_time, month; } col_idx = {-1, -1, -1, -1};
    
    for (size_t i = 0; i < headers.size(); ++i) {
        const string& h = headers[i];
        if (h == "ORIGIN") col_idx.origin = i;
        else if (h == "DEST") col_idx.dest = i;
        else if (h == "AIR_TIME") col_idx.air_time = i;
        else if (h == "MONTH") col_idx.month = i;
    }

    if (col_idx.origin == -1 || col_idx.dest == -1 || col_idx.air_time == -1 || col_idx.month == -1) {
        throw runtime_error("Missing required columns in header");
    }

    // Статистика
    unordered_map<int, unordered_map<string, int>> route_frequency;
    int total_routes = 0, filtered_routes = 0;

    // Первый проход: подсчет частот
    string line;
    while (getline(input_file, line)) {
        auto columns = parse_csv_line(line);
        if (columns.size() <= max({col_idx.origin, col_idx.dest, col_idx.air_time, col_idx.month})) continue;

        try {
            int month = stoi(columns[col_idx.month]);
            string origin = columns[col_idx.origin];
            string dest = columns[col_idx.dest];
            clean_quotes(origin);
            clean_quotes(dest);
            
            route_frequency[month][origin + "->" + dest]++;
            total_routes++;
        } catch (...) {
            continue;
        }
    }

    // Второй проход: фильтрация
    input_file.clear();
    input_file.seekg(0);
    getline(input_file, line); // Пропуск заголовка

    output_file << "ORIGIN,DEST,AIR_TIME\n";

    while (getline(input_file, line)) {
        auto columns = parse_csv_line(line);
        if (columns.size() <= max({col_idx.origin, col_idx.dest, col_idx.air_time, col_idx.month})) continue;

        try {
            string origin = columns[col_idx.origin];
            string dest = columns[col_idx.dest];
            string air_time_str = columns[col_idx.air_time];
            int month = stoi(columns[col_idx.month]);
            
            clean_quotes(origin);
            clean_quotes(dest);
            clean_quotes(air_time_str);
            
            int air_time = stoi(air_time_str);
            string route_key = origin + "->" + dest;

            if (air_time > 0 && route_frequency[month][route_key] >= min_frequency) {
                output_file << origin << "," << dest << "," << air_time << "\n";
                filtered_routes++;
            }
        } catch (...) {
            continue;
        }
    }

    // Вывод статистики
    cout << "Processing complete:\n"
         << "  Total routes processed: " << total_routes << "\n"
         << "  Routes after filtering: " << filtered_routes << "\n"
         << "  Filtering threshold: >= " << min_frequency << " flights/month\n";
}

int main(int argc, char* argv[]) {
    try {
        string input_file = "T_T100_SEGMENT_ALL_CARRIER.csv";
        string output_file = "filtered_file.csv";
        int min_frequency = 15;
        
        if (argc > 1) input_file = argv[1];
        if (argc > 2) output_file = argv[2];
        if (argc > 3) min_frequency = stoi(argv[3]);

        filter_csv(input_file, output_file, min_frequency);
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}