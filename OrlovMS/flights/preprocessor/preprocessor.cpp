#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>


enum CsvFields {
    CSV_PASSENGERS = 4,
    CSV_DISTANCE = 7,
    CSV_AIR_TIME = 9,
    CSV_ORIGIN_ID = 19,
    CSV_ORIGIN_CODE = 22,
    CSV_DEST_ID = 30,
    CSV_DEST_CODE = 33,
    CSV_STRING_ELEMENTS = 50,
};


std::vector<std::string> split(std::string str)
{
    std::vector<std::string> elements = {""};
    unsigned int element_cnt = 0;
    bool is_quoted = false;

    for(char ch : str) {
        switch(ch)
        {
        case ',':
            if(!is_quoted) {
                element_cnt++;
                elements.push_back("");
            }
            break;
        case '"':
            is_quoted = !is_quoted;

        default:
            elements[element_cnt].push_back(ch);
            break;
        }
    }

    return elements;
}


int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cout << "Wrong arguments\n";
        std::cout << "Usage: preprocessor <data file>.csv\n";
        return -1;
    }

    std::string filename(argv[1]);

    if(filename.substr(filename.find_last_of('.') + 1) != "csv") {
        std::cout << "Wrong file format\n";
        std::cout << ".csv file expected\n";
        return -1;
    }

    std::ifstream input_csv(filename);
    if(!input_csv.is_open()) {
        std::cout << "No such file: " << filename << std::endl;
        return -1;
    }

    std::map<int, std::map<int, float>> flights;
    std::map<std::string, int> airports;

    std::string csv_str;
    std::getline(input_csv, csv_str);  // Read description line

    while(std::getline(input_csv, csv_str)) {
        std::vector<std::string> elements = split(csv_str);

        float passangers = std::stof(elements[CSV_PASSENGERS]);
        float distance = std::stof(elements[CSV_DISTANCE]);
        float air_time = std::stof(elements[CSV_AIR_TIME]);
        int origin_id = std::stoi(elements[CSV_ORIGIN_ID]);
        int dest_id = std::stoi(elements[CSV_DEST_ID]);

        if(passangers < 0.001 || distance < 0.001 || air_time < 0.001) {
            continue;
        }

        if(!airports.count(elements[CSV_ORIGIN_CODE])) {
            airports[elements[CSV_ORIGIN_CODE]] = origin_id;
        }

        if(flights[origin_id].count(dest_id)) {
            if(air_time < flights[origin_id][dest_id]) {
                flights[origin_id][dest_id] = air_time;
            }
        }
        else {
            flights[origin_id][dest_id] = air_time;
        }
    }

    std::ofstream database_out("database.h");

    for(const auto& origin_list : flights) {
        database_out << "{" << origin_list.first << "," << std::endl << "{";
        for(const auto& dest : origin_list.second) {
            database_out << "{" << dest.first << ", " << dest.second << "}," << std::endl;
        }
        database_out << "}" << "}," << std::endl;
    }

    database_out.close();

    std::ofstream airports_out("airports.h");

    for(const auto& airport : airports) {
        airports_out << "{\"" << airport.first << "\", " << airport.second << "}," << std::endl;
    }

    airports_out.close();

    input_csv.close();

    return 0;
}