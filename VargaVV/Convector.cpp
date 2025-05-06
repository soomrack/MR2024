#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>

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

void filter_csv(const string& input_path, const string& output_path) {
    // Открытие файлов с проверкой
    ifstream input_file(input_path);
    if (!input_file.is_open()) {
        throw runtime_error("Ошибка: Не удалось открыть входной файл: " + input_path);
    }

    ofstream output_file(output_path);
    if (!output_file.is_open()) {
        input_file.close();
        throw runtime_error("Ошибка: Не удалось создать выходной файл: " + output_path);
    }

    try {
        // Обработка заголовка
        string header_line;
        if (!getline(input_file, header_line)) {
            throw runtime_error("Ошибка: Входной файл пуст");
        }
        
        vector<string> headers = parse_csv_line(header_line);
        
        // Поиск индексов колонок
        struct ColumnIndices {
            int origin = -1;
            int dest = -1;
            int air_time = -1;
            int month = -1;
        } col_idx;

        for (size_t i = 0; i < headers.size(); ++i) {
            if (headers[i] == "ORIGIN") col_idx.origin = i;
            else if (headers[i] == "DEST") col_idx.dest = i;
            else if (headers[i] == "AIR_TIME") col_idx.air_time = i;
            else if (headers[i] == "MONTH") col_idx.month = i;
        }

        // Проверка наличия всех колонок
        if (col_idx.origin == -1 || col_idx.dest == -1 || 
            col_idx.air_time == -1 || col_idx.month == -1) {
            throw runtime_error("Ошибка: В заголовке отсутствуют необходимые колонки");
        }

        // Первый проход: подсчёт частоты рейсов
        unordered_map<int, unordered_map<string, int>> route_frequency;
        string data_line;
        
        while (getline(input_file, data_line)) {
            vector<string> columns = parse_csv_line(data_line);
            
            // Пропуск некорректных строк
            if (columns.size() <= static_cast<size_t>(max({col_idx.origin, col_idx.dest, col_idx.air_time, col_idx.month}))) {
                continue;
            }

            try {
                int month = stoi(columns[col_idx.month]);
                string origin = columns[col_idx.origin];
                string dest = columns[col_idx.dest];

                clean_quotes(origin);
                clean_quotes(dest);

                route_frequency[month][origin + "->" + dest]++;

            } catch (const exception& e) {
                cerr << "\nПредупреждение: " << e.what() << endl;
                continue;
            }
        }

        // Второй проход: фильтрация и запись
        input_file.clear();
        input_file.seekg(0);
        getline(input_file, header_line); // Пропуск заголовка

        output_file << "ORIGIN,DEST,AIR_TIME\n"; // Запись нового заголовка

        while (getline(input_file, data_line)) {
            vector<string> columns = parse_csv_line(data_line);
            
            if (columns.size() <= static_cast<size_t>(max({col_idx.origin, col_idx.dest, col_idx.air_time, col_idx.month}))) {
                continue;
            }

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

                if (air_time > 0 && route_frequency[month][route_key] >= 15) {
                    output_file << origin << "," << dest << "," << air_time << "\n";
                }
            } catch (const exception& e) {
                cerr << "\nПредупреждение: " << e.what() << endl;
                continue;
            }
        }
    } catch (...) {
        // Закрытие файлов при возникновении исключения
        input_file.close();
        output_file.close();
        throw;
    }

    // Закрытие файлов при нормальном завершении
    input_file.close();
    if (input_file.fail()) {
        cerr << "Предупреждение: Не удалось корректно закрыть входной файл" << endl;
    }
    
    output_file.close();
    if (output_file.fail()) {
        cerr << "Предупреждение: Не удалось корректно закрыть выходной файл" << endl;
    }
}

int main() {
    try {
        filter_csv("T_T100_SEGMENT_ALL_CARRIER.csv", "filtered_data.csv");
        cout << "Обработка данных успешно завершена!" << endl;
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}