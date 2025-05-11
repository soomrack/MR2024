#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// Структура для хранения информации об аэропорте
struct AirportInfo {
    std::string code;
    std::string city;
    std::string state;
    std::string country;
    int normalized_id;
};

// Функция для записи данных в CSV файл
void write_csv(const std::string& filename, const std::vector<std::string>& lines) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
    }

    for (const auto& line : lines) {
        outfile << line << "\n";
    }

    std::cout << "Файл " << filename << " успешно создан (" << lines.size() << " строк)\n";
}

// Основная функция обработки данных
void process_data(const std::string& input_filename) {
    std::ifstream infile(input_filename);
    if (!infile.is_open()) {
        throw std::runtime_error("Не удалось открыть входной файл: " + input_filename);
    }

    std::unordered_map<std::string, AirportInfo> airports;
    std::vector<std::string> graph_lines;
    std::vector<std::string> mapping_lines;

    // Заголовки для файлов
    graph_lines.push_back("from,to,distance");
    mapping_lines.push_back("original_code,normalized_id,city_state");

    int airport_id = 1;
    std::string line;
    size_t line_num = 0;

    // Пропускаем заголовок
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        line_num++;
        if (line.empty()) continue;

        try {
            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            bool in_quotes = false;
            std::string current_token;

            // Парсинг CSV с учетом кавычек
            for (char c : line) {
                if (c == '"') {
                    in_quotes = !in_quotes;
                }
                else if (c == ',' && !in_quotes) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
                else {
                    current_token += c;
                }
            }
            tokens.push_back(current_token);

            if (tokens.size() < 40) {
                std::cerr << "Пропуск строки " << line_num << ": недостаточно данных (" << tokens.size() << " полей)\n";
                continue;
            }

            // Извлекаем данные аэропортов
            std::string origin_code = tokens[17]; // ORIGIN
            std::string dest_code = tokens[18];   // DEST

            // Обрабатываем аэропорт отправления
            if (airports.find(origin_code) == airports.end()) {
                AirportInfo origin_info;
                origin_info.code = origin_code;
                origin_info.city = tokens[7];  // ORIGIN_CITY_NAME
                origin_info.state = tokens[9];  // ORIGIN_STATE_ABR
                origin_info.country = tokens[12]; // ORIGIN_COUNTRY_NAME
                origin_info.normalized_id = airport_id++;
                airports[origin_code] = origin_info;

                // Добавляем в mapping
                mapping_lines.push_back(
                    origin_code + "," +
                    std::to_string(origin_info.normalized_id) + "," +
                    origin_info.city + ", " + origin_info.state
                );
            }

            // Обрабатываем аэропорт назначения
            if (airports.find(dest_code) == airports.end()) {
                AirportInfo dest_info;
                dest_info.code = dest_code;
                dest_info.city = tokens[19]; // DEST_CITY_NAME
                dest_info.state = tokens[21]; // DEST_STATE_ABR
                dest_info.country = tokens[24]; // DEST_COUNTRY_NAME
                dest_info.normalized_id = airport_id++;
                airports[dest_code] = dest_info;

                // Добавляем в mapping
                mapping_lines.push_back(
                    dest_code + "," +
                    std::to_string(dest_info.normalized_id) + "," +
                    dest_info.city + ", " + dest_info.state
                );
            }

            // Извлекаем расстояние (защита от ошибок)
            int distance = 0;
            try {
                distance = std::stoi(tokens[37]); // DISTANCE
            }
            catch (...) {
                std::cerr << "Ошибка парсинга расстояния в строке " << line_num << "\n";
                continue;
            }

            // Добавляем рейс в граф
            graph_lines.push_back(
                std::to_string(airports[origin_code].normalized_id) + "," +
                std::to_string(airports[dest_code].normalized_id) + "," +
                std::to_string(distance)
            );

        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка обработки строки " << line_num << ": " << e.what() << "\n";
        }

        // Прогресс
        if (line_num % 100000 == 0) {
            std::cout << "Обработано строк: " << line_num << "\n";
        }
    }

    // Сохраняем результаты
    write_csv("airline_graph.csv", graph_lines);
    write_csv("airport_mapping.csv", mapping_lines);

    std::cout << "Обработка завершена. Всего аэропортов: " << airports.size()
        << ", рейсов: " << graph_lines.size() - 1 << "\n";
}

int main() {
    try {
        std::cout << "Программа предварительной обработки данных авиарейсов\n";

        // Проверяем существование выходных файлов
        if (fs::exists("airline_graph.csv") && fs::exists("airport_mapping.csv")) {
            std::cout << "Выходные файлы уже существуют. Удалите их для повторной обработки.\n";
            return 0;
        }

        // Обрабатываем данные
        process_data("T_T100_SEGMENT_ALL_CARRIER_part.csv");

        std::cout << "Готово! Созданы файлы:\n"
            << "- airline_graph.csv\n"
            << "- airport_mapping.csv\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}