#include "preprocessor.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

void process_csv_file(const std::string& filename) {
    std::ifstream input_csv(filename);
    if (!input_csv.is_open()) {
        std::cout << "No such file: " << filename << std::endl;
        return;
    }

    std::map<AirportId, std::map<AirportId, float>> flights;
    std::map<AirportCode, AirportId> airports;

    std::string csv_str;
    std::getline(input_csv, csv_str); // Read description line

    while (std::getline(input_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        int passengers = std::stoi(elements[CSV_PASSENGERS]);
        float distance = std::stof(elements[CSV_DISTANCE]);
        float air_time = std::stof(elements[CSV_AIR_TIME]);
        AirportId origin_id = std::stoi(elements[CSV_ORIGIN_ID]);
        AirportId dest_id = std::stoi(elements[CSV_DEST_ID]);

        if (passengers == 0 || distance < 0.001 || air_time < 0.001) {
            continue;
        }

        if (!airports.count(elements[CSV_ORIGIN_CODE])) {
            airports[elements[CSV_ORIGIN_CODE]] = origin_id;
        }

        if (flights[origin_id].count(dest_id)) {
            if (air_time < flights[origin_id][dest_id]) {
                flights[origin_id][dest_id] = air_time;
            }
        } else {
            flights[origin_id][dest_id] = air_time;
        }
    }

    std::ofstream database_out("database.csv");
    for (const auto& origin_list : flights) {
        for (const auto& dest : origin_list.second) {
            database_out << origin_list.first << "," << dest.first << "," << dest.second << std::endl;
        }
    }
    database_out.close();

    std::ofstream airports_out("airports.csv");
    for (const auto& airport : airports) {
        airports_out << airport.first << "," << airport.second << std::endl;
    }
    airports_out.close();

    input_csv.close();
}

std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> elements = {""};
    unsigned int element_cnt = 0;
    bool is_quoted = false;

    for (char ch : str) {
        switch (ch) {
        case ',':
            if (!is_quoted) {
                element_cnt++;
                elements.push_back("");
            }
            break;
        case '"':
            is_quoted = !is_quoted;
            break;
        default:
            elements[element_cnt].push_back(ch);
            break;
        }
    }

    return elements;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Wrong arguments\n";
        std::cout << "Usage: preprocessor <data file>.csv\n";
        return -1;
    }

    std::string filename(argv[1]);

    if (filename.substr(filename.find_last_of('.') + 1) != "csv") {
        std::cout << "Wrong file format\n";
        std::cout << ".csv file expected\n";
        return -1;
    }

    process_csv_file(filename);

    return 0;
}

