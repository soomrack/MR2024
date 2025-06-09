#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <map>
#include <string>
#include <vector>

using AirportCode = std::string;
using AirportId = int;

struct Node {
    float min_time;
    bool processed;
    AirportId prev_node;
};

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

void process_csv_file(const std::string& filename);

std::vector<std::string> split(const std::string& str);

#endif // PREPROCESSOR_H

