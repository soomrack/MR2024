#include "parser.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

vector<string> Parser::split_line(const string& line)
{
    vector<string> fields;
    string current;
    bool is_in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];

        if (ch == '"') {
            is_in_quotes = !is_in_quotes;
        } else if (ch == ',' && !is_in_quotes) {
            fields.push_back(current);
            current.clear();
        } else if (ch == ',' && is_in_quotes) {
            while (line[i + 1] != '"') ++i;
        } else {
            current += ch;
        }
    }

    fields.push_back(current);

    return fields;
}


Flight Parser::parse_line(const vector<string>& fields)
{
    Flight f;
    f.origin_airport = fields[22];  // ORIGIN
    f.origin_city = fields[23];     // ORIGIN_CITY_NAME
    f.dest_airport = fields[33];    // DEST
    f.dest_city = fields[34];       // DEST_CITY_NAME
    f.air_time = stod(fields[8]) / stod(fields[0]);   // RAMP_TO_RAMP_TIME, in minutes

    return f;
}


vector<Flight> Parser::parse(const string& src_file)
{
    vector<Flight> flights;
    ifstream file(src_file);

    if (!file.is_open()) {
        cerr << "File opening error" << endl;
        return flights;
    }

    string line;

    getline(file, line); 

    while(getline(file, line)) {
        vector<string> fields = split_line(line);

        if (fields.size() < 35) continue;

        if (stod(fields[8]) == 0) continue;

        if (stoi(fields[0]) == 0) continue;

        Flight flight = parse_line(fields);
        flights.push_back(flight);
    }

    return flights;
}
