#pragma once

#include <vector>
#include <string>

struct FlightRecord {
    std::string origin_airport;
    std::string origin_city;
    std::string dest_airport;
    std::string dest_city;
    int air_time;
};

class CsvFlightParser {
public:
    static std::vector<FlightRecord> parse(const std::string& filename);

private:
    static FlightRecord parse_line(const std::string& line);
    static std::vector<std::string> split_line(const std::string& line);
    static int safe_string_to_int(const std::string& str);
};