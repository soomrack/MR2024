#include "csv_flight_parser.hpp"
#include "logger.hpp"
#include "error_handler.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#define log(level, message) logger.log(level, "csv_flight_parser", message)

Logger& logger = Logger::get_instance();

std::vector<FlightRecord> CsvFlightParser::parse(const std::string& filename) {
    std::vector<FlightRecord> records;
    std::ifstream file(filename);

    if (!file.is_open()) {
        log(LogLevel::ERROR, "Failed to open file: " + filename);
        throw FileError("Failed to open file: " + filename);
    }

    std::string line;
    size_t line_number = 0;

    // Пропуск заголовка
    std::getline(file, line);
    line_number++;

    while (std::getline(file, line)) {
        line_number++;
        try {
            records.push_back(parse_line(line));
        }
        catch (const std::exception& e) {
            log(LogLevel::ERROR, "Skipping line " + std::to_string(line_number) + 
                ": " + e.what());
        }
    }
    return records;
}

std::vector<std::string> CsvFlightParser::split_line(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;
    char prev = '\0';

    for (char c : line) {
        if (c == '"' && prev != '\\') {
            in_quotes = !in_quotes;
        }
        else if (c == '\\' && prev == '\\') {
            prev = '\0';
            continue;
        }
        else if (c == ',' && !in_quotes) {
            fields.push_back(field);
            field.clear();
        }
        else {
            if (c != '\"') {
                field += c;
            }
        }
        prev = c;
    }

    fields.push_back(field);
    return fields;
}

FlightRecord CsvFlightParser::parse_line(const std::string& line) {
    auto fields = split_line(line);

    if (fields.size() < 38) {
        throw CSVParsingError("CSV line has fewer than 38 columns");
    }

    return {
        .origin_airport = fields[22],
        .origin_city = fields[23],
        .dest_airport = fields[33],
        .dest_city = fields[34],
        .air_time = safe_string_to_int(fields[9])
    };
}

int CsvFlightParser::safe_string_to_int(const std::string& str) {
    if (str.empty()) {
        throw DataError("Empty air_time value");
    }

    try {
        return std::stoi(str);
    }
    catch (const std::exception& e) {
        throw DataError("Invalid air_time format: " + std::string(e.what()));
    }
}