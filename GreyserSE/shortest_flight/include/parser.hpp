#pragma once

#include <vector>
#include <string>

using namespace std;


typedef struct Flight {
    string origin_airport;
    string origin_city;
    string dest_airport;
    string dest_city;
    int air_time;
} Flight;


class Parser {
private:
    vector<string> split_line(const string& line);
    Flight parse_line(const vector<string>& fields);
    string normalize_city_name(string& city_name);
public:
    vector<Flight> parse(const string& src_file);
};
