#include "Parser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;


vector<Flight> Parser::parse_data(const string& data_filename)
{
    vector<Flight> flights;
    string line; 
    ifstream data(data_filename);

    getline(data, line); // пропуск первой строки

    while(getline(data, line)) {
        if (line[0] != '0') flights.push_back(parse_line(line));
    }

    return flights;    
};



Flight Parser::parse_line(const string& line)
{
    vector<string> words;
    string word;
    bool inQuotes = false;
    bool ignore = true;

    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == '"') {
            inQuotes = !inQuotes;
        } else if (line[i] == ',' && !inQuotes) {
            words.push_back(word);
            word.clear();
        } else if (line[i] == ',' && inQuotes) {
            while (line[i + 1] != '"') ++i;
        } else {
            word += line[i];
        }
    }

    if (words.size() < 35) {
        throw runtime_error("Invalid data format in line: " + line);
    }

    return {
        .origign_airport = words[22],
        .origin_city = words[23],
        .dest_airport = words[33],
        .dest_city = words[34],
        .air_time = words[9].empty() || words[0].empty() ? 0 : stoi(words[9])/stoi(words[0]);
    };
};



// int main(){
//     Parser parser;
//     vector<Flight> flights = parser.parse_data("data.csv");

//     for(size_t i = 1000; i < 1025; i++) {
//         cout << flights[i].origin_city << ", "
//         << flights[i].origign_airport << ", "
//         << flights[i].dest_city << ", "
//         << flights[i].dest_airport << ", "
//         << flights[i].air_time << "\n";
//     }
    
//     return 0;
// };