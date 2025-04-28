#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>
#include <stdexcept>

namespace fpp {

using airportId = int;
using airTime = double;
using flightPath = std::pair<std::vector<airportId>, std::vector<airTime>>;

/**
 * @brief Синглтон-класс для управления графом рейсов и поиском путей.
 */
class FlightPathPlanner {
private:
    /**
     * @brief Внутренний класс для хранения информации о рейсе.
     */
    class FlightInfo {
    private:
        airTime m_flightAirTime;
        airportId m_originAirportId;
        airportId m_destAirportId;
        int m_year;
        int m_quarter;
        int m_month;

    public:
        FlightInfo(airTime airTime, airportId origin, airportId dest, int yr, int qtr, int mth)
            : m_flightAirTime(airTime), m_originAirportId(origin), m_destAirportId(dest),
              m_year(yr), m_quarter(qtr), m_month(mth) {}

        airTime getFlightAirTime() const { return m_flightAirTime; }
        airportId getOriginAirportId() const { return m_originAirportId; }
        airportId getDestAirportId() const { return m_destAirportId; }
        int getYear() const { return m_year; }
        int getQuarter() const { return m_quarter; }
        int getMonth() const { return m_month; }
    };

    using FlightGraph = std::map<airportId, std::vector<std::pair<airportId, FlightInfo>>>;
    using AirportsSet = std::unordered_set<airportId>;
    using DistanceMap = std::unordered_map<airportId, airTime>;
    using PathInfo = std::pair<airportId, airTime>;
    using PathMap = std::unordered_map<airportId, PathInfo>;

    FlightGraph flightGraph;
    AirportsSet airports;

    FlightPathPlanner() = default;

	/**
	 * @brief Парсит строку CSV с учетом кавычек.
	 * @param line Входная строка CSV.
	 * @return Вектор полей строки.
	 */
    std::vector<std::string> parseCsvLine(const std::string& line);

	/**
	 * @brief Выполняет алгоритм Дейкстры для поиска кратчайшего пути.
	 * @param originIndex ID аэропорта отправления.
	 * @return Пара: карта расстояний и карта путей.
	 */
    std::pair<DistanceMap, PathMap> dijkstra(airportId originIndex);

	/**
	 * @brief Восстанавливает путь из результатов алгоритма Дейкстры.
	 * @param pathInfo Карта путей.
	 * @param destIndex ID аэропорта назначения.
	 * @return Пара векторов: ID аэропортов и время в воздухе.
	 */
    flightPath reconstructPath(const PathMap& pathInfo, airportId destIndex);

public:
    static FlightPathPlanner& getInstance();

    /**
     * @brief Загружает данные о рейсах из CSV-файла.
     * @param filePath Путь к CSV-файлу.
     * @throws FlightPathPlannerException если файл не может быть открыт или данные недействительны.
     */
    void loadData(const std::string& filePath);

    /**
     * @brief Очищает граф рейсов и множество аэропортов.
     */
    void clear();

    /**
     * @brief Проверяет, существует ли аэропорт в графе.
     * @param index ID аэропорта для проверки.
     * @return True, если аэропорт существует, иначе false.
     */
    bool containsAirport(airportId index) const;

    /**
     * @brief Находит кратчайший путь между двумя аэропортами.
     * @param originIndex ID аэропорта отправления.
     * @param destIndex ID аэропорта назначения.
     * @return Пара векторов: ID аэропортов и время в воздухе. Пусто, если путь не существует.
     */
    flightPath findFlightPathBetween(airportId originIndex, airportId destIndex);

    // Удалены конструктор копирования и оператор присваивания для синглтона
    FlightPathPlanner(const FlightPathPlanner&) = delete;
    FlightPathPlanner& operator=(const FlightPathPlanner&) = delete;
};

/**
 * @brief Класс исключений для ошибок FlightPathPlanner.
 */
class FlightPathPlannerException : public std::exception {
private:
    std::string m_errorMessage;

public:
    explicit FlightPathPlannerException(const std::string& message) : m_errorMessage(message) {}
    const char* what() const noexcept override { return m_errorMessage.c_str(); }
};

} // namespace fpp
