#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <stdexcept>

#include "variables.cpp"


std::vector<std::string> separation(const std::string &line, const char delimiter_sign, const char exclusion_sign)
{
    std::vector<std::string> tokens;
    std::string token;
    bool in_exclusion = false;
    
    for (int index = 0; index < line.size(); index++) {
        if (line[index] == exclusion_sign) {
            in_exclusion = !in_exclusion;
        }
        else if (line[index] == delimiter_sign && (in_exclusion == false)) {
            tokens.push_back(token);
            token.clear();  
        }
        else {
            token += line[index];
        }
    }

    tokens.push_back(token);
    return tokens;
}


std::string word_before_separator(const std::string& str,  const char delimiter_sign) {
    size_t position = str.find(delimiter_sign);

    if (position != std::string::npos) {
        return str.substr(0, position);
    }

    return str;
}


bool check_validity_air_time(const std::string& air_time_str)
{
    try {
        float air_time_float = std::stof(air_time_str);
        return air_time_float != 0.0f;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}


void intermediate_csv_file() {
    std::ifstream in_file;
    in_file.open("T_T100_SEGMENT_ALL_CARRIER.csv");

    if (in_file.is_open() == false) {
        std::cerr << "Исходный файл не открылся" << std::endl;
        return;
    }

    std::ofstream out_file;
    out_file.open("airoport_sorting_intermediate.csv");

    if (out_file.is_open() == false) {
        std::cerr << "Промежуточный файл не открылся" << std::endl;
        return;
    }

    std::string line;

    while (getline(in_file, line)) {
        std::vector<std::string> cell = separation(line, ',', '"');

        if ((cell.size() >= DEST_CITY_NAME) && (check_validity_air_time(cell[AIR_TIME]) == true)) {
            std::string origin_sity = word_before_separator(cell[ORIGIN_CITY_NAME], ',');
            std::string dest_sity = word_before_separator(cell[DEST_CITY_NAME], ',');
            
            out_file << cell[DISTANCE] << ","
                    << cell[AIR_TIME] << ","
                    << cell[ORIGIN] << "," 
                    << origin_sity << "," 
                    << cell[DEST] << "," 
                    << dest_sity << std::endl; 
        }
        else {
            std::cerr << "Ячеек недостаточно:" << line << std::endl;
        }
    }

    in_file.close();
    out_file.close();
}


void sorting_periodicity_file(unsigned int periodicity)
{
    std::map<std::pair<std::string, std::string>, int> count_flight;

    std::ifstream in_file;
    in_file.open("airoport_sorting_intermediate.csv");

    if (in_file.is_open() == false) {
        std::cerr << "Промежуточный файл не открылся" << std::endl;
        return;
    }

    std::string line;
    while (getline(in_file, line)) {
        std::vector<std::string> cell = separation(line, ',', '"'); 
        
        if (cell.size() >=6) {
            std::string from = cell[2];
            std::string to = cell[5];

            count_flight[{from, to}]++;
        }
    }
    in_file.close();

    in_file.open("airoport_sorting_intermediate.csv");

    if (in_file.is_open() == false) {
        std::cerr << "Промежуточный файл не открылся" << std::endl;
        return;
    }

    std::ofstream out_file;
    out_file.open("airoport_sorting.csv");

    if (out_file.is_open() == false) {
        std::cerr << "Конечный файл не открылся" << std::endl;
        return;
    }

    while (getline(in_file, line)) {
        std::vector<std::string> cell = separation(line, ',', '"'); 
        
        if (cell.size() >=6) {
            std::string from = cell[2];
            std::string to = cell[5];

            if (count_flight[{from, to}] >= periodicity) {
                out_file << line << std::endl;
            }
        }
    }

    in_file.close();
    out_file.close();

    remove("airoport_sorting_intermediate.csv");    
}
