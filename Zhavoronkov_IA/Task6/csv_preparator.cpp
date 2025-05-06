#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <stdexcept>

#include "definition.cpp"


std::vector<std::string> split(const std::string &line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
    for (char c : line) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == delimiter && !in_quotes) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}


std::string get_first_word(const std::string& s) {
    size_t comma_pos = s.find(',');
    if (comma_pos != std::string::npos) {
        return s.substr(0, comma_pos);
    }
    return s;
}


void make_usefull_csv() {
    std::ifstream input_file("full.csv");
    std::ofstream output_file("recive.csv");

    if (!input_file.is_open()) {
        std::cerr << "Unable to open full.csv" << std::endl;
        return;
    }
    
    if (!output_file.is_open()) {
        std::cerr << "Unable to make recive.csv" << std::endl;
        return;
    }

    std::string line;
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        
        if (cells.size() >= MONTH) {
            std::string origin_city = get_first_word(cells[ORIGIN_CITY_NAME]);
            std::string dest_city = get_first_word(cells[DEST_CITY_NAME]);
            
            output_file << cells[DISTANCE] << "," 
                        << cells[AIR_TIME] << "," 
                        << cells[ORIGIN] << "," 
                        << origin_city << ","
                        << cells[DEST] << "," 
                        << dest_city << std::endl;
        } else {
            std::cerr << "Not enough cells in a line: " << line << std::endl;
        }
    }
    
    input_file.close();
    output_file.close();
    std::cout << "File recive.csv was created successfully" << std::endl;
}


bool is_air_time_valid(const std::string& air_time_str) {
    try {
        float air_time = std::stof(air_time_str);
        return air_time != 0.0f;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}


void count_in_csv(unsigned int periodicity) {
    std::map<std::pair<std::string, std::string>, int> flightCounter;

    // Первый проход: подсчёт только рейсов с AIR_TIME != 0
    std::ifstream input_file("recive.csv");
    if (!input_file.is_open()) {
        std::cerr << "Unable to open recive.csv" << std::endl;
        return;
    }
    
    std::string line;
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (cells.size() >= 6 && is_air_time_valid(cells[1])) {
            std::string from = cells[2];
            std::string to = cells[5];
            flightCounter[{from, to}]++;
        }
    }
    input_file.close();
    
    // Второй проход: запись только рейсов с AIR_TIME != 0 и достаточной периодичностью
    input_file.open("recive.csv");
    std::ofstream output_file("output.csv");
        
    if (!input_file.is_open() || !output_file.is_open()) {
        std::cerr << "Unable to open files" << std::endl;
        return;
    }
    
    while (getline(input_file, line)) {
        std::vector<std::string> cells = split(line, ',');
        if (cells.size() >= 6 && is_air_time_valid(cells[1])) {
            std::string from = cells[2];
            std::string to = cells[5];
            if (flightCounter[{from, to}] >= periodicity) {
                output_file << line << std::endl;
            }
        }
    }
    
    input_file.close();
    output_file.close();
    remove("recive.csv");
    std::cout << "Analysis is completed. Results are in output.csv" << std::endl;
}
