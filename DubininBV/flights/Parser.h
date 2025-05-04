#pragma once

#include <vector>
#include <string>

struct Flight {
    std::string origign_airport;
    std::string origin_city;
    std::string dest_airport;
    std::string dest_city;
    int air_time;
};

class Parser {
public: 
    std::vector<Flight> parse_data(const std::string& data_filename);
private:
    Flight parse_line(const std::string& line);
};
