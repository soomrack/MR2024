#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include "flightpathplanner.hpp"

namespace fpp {

FlightPathPlanner& FlightPathPlanner::getInstance() {
    static FlightPathPlanner instance;
    return instance;
}

std::vector<std::string> FlightPathPlanner::parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            if (!field.empty() && field.front() == '"' && field.back() == '"') {
                field = field.substr(1, field.size() - 2);
            }
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    if (!field.empty()) {
        if (field.front() == '"' && field.back() == '"') {
            field = field.substr(1, field.size() - 2);
        }
        fields.push_back(field);
    }
    return fields;
}

void FlightPathPlanner::loadData(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw FlightPathPlannerException("Не удалось открыть файл: " + filePath);
    }

    std::string line;

    if (!std::getline(file, line)) {
        throw FlightPathPlannerException("Пустой файл: " + filePath);
    }

    auto headers = parseCsvLine(line);
    std::map<std::string, size_t> headerIndices;
    for (size_t i = 0; i < headers.size(); ++i) {
        headerIndices[headers[i]] = i;
    }

    std::vector<std::string> requiredFields = {
        "AIR_TIME", "ORIGIN_AIRPORT_ID", "DEST_AIRPORT_ID", "YEAR", "QUARTER", "MONTH"
    };
    for (const auto& field : requiredFields) {
        if (headerIndices.find(field) == headerIndices.end()) {
            throw FlightPathPlannerException("Отсутствует необходимое поле в заголовке: " + field);
        }
    }

    while (std::getline(file, line)) {
        auto fields = parseCsvLine(line);
        if (fields.size() < headerIndices.size()) {
            throw FlightPathPlannerException("Неверное количество полей в строке: " + line);
        }

        try {
            airTime airTime = std::stod(fields[headerIndices["AIR_TIME"]]);
            airportId origin = std::stoi(fields[headerIndices["ORIGIN_AIRPORT_ID"]]);
            airportId dest = std::stoi(fields[headerIndices["DEST_AIRPORT_ID"]]);
            int year = std::stoi(fields[headerIndices["YEAR"]]);
            int quarter = std::stoi(fields[headerIndices["QUARTER"]]);
            int month = std::stoi(fields[headerIndices["MONTH"]]);

            if (airTime <= 0) {
                continue;
            }

            FlightInfo flight(airTime, origin, dest, year, quarter, month);
            flightGraph[origin].push_back({dest, flight});
            airports.insert(origin);
            airports.insert(dest);
        } catch (const std::exception& e) {
            throw FlightPathPlannerException("Неверный формат данных в строке: " + line);
        }
    }
}

void FlightPathPlanner::clear() {
    flightGraph.clear();
    airports.clear();
}

bool FlightPathPlanner::containsAirport(airportId index) const {
    return airports.count(index) > 0;
}

std::pair<FlightPathPlanner::DistanceMap, FlightPathPlanner::PathMap> FlightPathPlanner::dijkstra(airportId originIndex) {
    DistanceMap distances;
    PathMap pathInfo;
    auto cmp = [](const std::pair<airTime, airportId>& a, const std::pair<airTime, airportId>& b) {
        return a.first > b.first;
    };
    std::priority_queue<std::pair<airTime, airportId>, std::vector<std::pair<airTime, airportId>>, decltype(cmp)> pq(cmp);

    distances[originIndex] = 0.0;
    pq.push({0.0, originIndex});

    while (!pq.empty()) {
        auto [dist, u] = pq.top();
        pq.pop();
        if (dist > distances[u]) continue;

        for (const auto& [v, flight] : flightGraph[u]) {
            double newDist = distances[u] + flight.getFlightAirTime();
            if (distances.find(v) == distances.end() || newDist < distances[v]) {
                distances[v] = newDist;
                pathInfo[v] = {u, flight.getFlightAirTime()};
                pq.push({newDist, v});
            }
        }
    }
    return {distances, pathInfo};
}

flightPath FlightPathPlanner::reconstructPath(const PathMap& pathInfo, airportId destIndex) {
    std::vector<airportId> airports;
    std::vector<airTime> times;
    airportId current = destIndex;

    while (pathInfo.find(current) != pathInfo.end()) {
        airports.push_back(current);
        auto [parent, time] = pathInfo.at(current);
        times.push_back(time);
        current = parent;
    }
    airports.push_back(current);

    if (!containsAirport(current)) {
        return {{}, {}};
    }

    std::reverse(airports.begin(), airports.end());
    std::reverse(times.begin(), times.end());
    return {airports, times};
}

flightPath FlightPathPlanner::findFlightPathBetween(airportId originIndex, airportId destIndex) {
    if (!containsAirport(originIndex) || !containsAirport(destIndex)) {
        return {{}, {}};
    }
    auto [distances, pathInfo] = dijkstra(originIndex);
    if (distances.find(destIndex) == distances.end()) {
        return {{}, {}};
    }
    return reconstructPath(pathInfo, destIndex);
}

} // namespace fpp
