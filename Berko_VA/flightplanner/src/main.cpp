#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "flightpathplanner.hpp"

std::string getFilePath() {
    std::string filePath;

    FILE* pipe = popen("zenity --file-selection --file-filter='CSV files | *.csv' --title='Выберите CSV-файл'", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            filePath = buffer;
            filePath.erase(filePath.find_last_not_of("\n\r") + 1);
        }
        pclose(pipe);
    }

    if (!filePath.empty()) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            file.close();
            return filePath;
        }
    }

    // Запасной вариант: ввод пути через консоль
    std::cout << "Не удалось выбрать файл через графический интерфейс или файл недоступен.\n";
    while (true) {
        std::cout << "Введите путь к CSV-файлу: ";
        std::getline(std::cin, filePath);
        std::ifstream file(filePath);
        if (file.is_open()) {
            file.close();
            return filePath;
        }
        std::cout << "Файл не найден. Попробуйте снова.\n";
    }
}


fpp::airportId getAirportIndex(const std::string& prompt, const fpp::FlightPathPlanner& planner) {
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);
        try {
            int id = std::stoi(line);
            if (planner.containsAirport(id)) {
                return id;
            }
            std::cout << "ID аэропорта не найден. Попробуйте снова.\n";
        } catch (const std::exception&) {
            std::cout << "Неверный ввод. Введите целое число.\n";
        }
    }
}

void printFlightPath(const fpp::flightPath& path) {
    const auto& [airports, times] = path;
    if (airports.empty()) {
        std::cout << "Путь не найден.\n";
        return;
    }

    double totalAirTime = 0.0;
    for (size_t i = 0; i < airports.size() - 1; ++i) {
        std::cout << "Аэропорт " << airports[i] << " -> Аэропорт " << airports[i + 1]
                  << " (" << times[i] << " часов)\n";
        totalAirTime += times[i];
    }
    std::cout << "Общее время в воздухе: " << totalAirTime << " часов\n";
}

int main() {
    fpp::FlightPathPlanner& planner = fpp::FlightPathPlanner::getInstance();

    while (true) {
        try {
            std::string filePath = getFilePath();
            planner.loadData(filePath);
            break;
        } catch (const fpp::FlightPathPlannerException& e) {
            std::cout << "Ошибка: " << e.what() << "\nПопробуйте снова.\n";
        }
    }

    auto origin = getAirportIndex("Введите ID аэропорта отправления: ", planner);
    auto dest = getAirportIndex("Введите ID аэропорта назначения: ", planner);

    auto path = planner.findFlightPathBetween(origin, dest);
    printFlightPath(path);

    planner.clear();
    return 0;
}

