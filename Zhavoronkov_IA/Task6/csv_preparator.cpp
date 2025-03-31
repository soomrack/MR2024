#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "definition.cpp"


std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


void make_usefull_csv()
{
    std::ifstream input_file;
    input_file.open("full.csv");
    std::ofstream output_file;
    output_file.open("recive.csv");

    if (!input_file.is_open()) {
        std::cerr << "Не удалось открыть файл full.csv" << std::endl;
        return;
    }
    
    if (!output_file.is_open()) {
        std::cerr << "не удалось создать файл recive.csv" << std::endl;
        return;
    }


    std::string line;
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        
        // Проверяем, что в строке достаточно ячеек
        if (cells.size() >= 50) {
            // Получаем ячейки DISTANCE, AIR_TIME, ORIGIN, DEST, YEAR, MONTH
            output_file << cells[DISTANCE] << "," 
                        << cells[AIR_TIME] << "," 
                        << cells[ORIGIN] << "," 
                        << cells[DEST + 1] << "," 
                        << cells[YEAR + 2] << "," 
                        << cells[MONTH + 2] <<std::endl;
        } else {
            std::cerr << "В строке недостаточно ячеек: " << line << std::endl;
        }
    }
    
    input_file.close();
    output_file.close();
    
    std::cout << "Файл recive.csv успешно создан" << std::endl;
    
    return;
}


void count_in_csv(unsigned int periodicity)
{
    // Создаем контейнер для подсчета пар аэропортов
    std::map<std::pair<std::string, std::string>, int> flightCounter;

    // Первый проход: подсчет количества перелетов
    std::ifstream input_file("recive.csv");
    if (!input_file.is_open()) {
        std::cerr << "Ошибка открытия файла recive.csv" << std::endl;
        return;
    }
    
    std::string line;
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (cells.size() >= 4) { // Проверяем наличие всех необходимых колонок
            std::string from = cells[2]; // Позиция 2
            std::string to = cells[3];   // Позиция 3
            flightCounter[{from, to}]++;
        }
    }
    input_file.close();
    
    // Второй проход: запись строк с популярными маршрутами
    input_file.open("recive.csv");
    std::ofstream output_file("output.csv");
        
    if (!input_file.is_open() || !output_file.is_open()) {
        std::cerr << "Ошибка открытия файлов" << std::endl;
        return;
    }
    
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (cells.size() >= 4) {
            std::string from = cells[2];
            std::string to = cells[3];
                
            // Проверяем популярность маршрута
            if (flightCounter[{from, to}] >= periodicity) {
                output_file << line << std::endl;
            }
        }
    }
    
    input_file.close();
    output_file.close();
    remove("recive.csv");
    
    std::cout << "Анализ завершен. Результаты в output.csv" << std::endl;
    return;
}


int main()
{
    make_usefull_csv();
    count_in_csv(250);
}
