/// @author Вадим Берко
/// @date 2025-04-28
/// @version 1.0.0

#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>
#include <stdexcept>

/**
 * @brief Пространство имен для планировщика маршрутов авиаперелетов
 */
namespace fpp {

using airportId = int; /**< Псевдоним для идентификатора аэропорта */
using airTime = double; /**< Псевдоним для времени в воздухе (в часах) */
using flightPath = std::pair<std::vector<airportId>, std::vector<airTime>>; /**< Псевдоним для пути: пара векторов аэропортов и времен перелетов */

/**
 * @brief Синглтон-класс для управления графом рейсов и поиском кратчайших путей
 *
 * Реализует функционал загрузки данных о рейсах из CSV-файла, построения графа рейсов,
 * поиска кратчайшего пути между аэропортами с использованием алгоритма Дейкстры
 * и управления данными о рейсах и аэропортах.
 */
class FlightPathPlanner {
private:
    /**
     * @brief Внутренний класс для хранения информации о рейсе
     *
     * Содержит данные о времени в воздухе, аэропортах отправления и назначения,
     * а также временных характеристиках рейса (год, квартал, месяц).
     */
    class FlightInfo {
    private:
        airTime m_flightAirTime; 		/**< Время в воздухе для рейса */
        airportId m_originAirportId; 	/**< Идентификатор аэропорта отправления */
        airportId m_destAirportId; 		/**< Идентификатор аэропорта назначения */
        int m_year; 					/**< Год выполнения рейса */
        int m_quarter; 					/**< Квартал выполнения рейса */
        int m_month; 					/**< Месяц выполнения рейса */

    public:
        /**
         * @brief Конструктор для создания информации о рейсе
         * @param airTime Время в воздухе (в часах)
         * @param origin Идентификатор аэропорта отправления
         * @param dest Идентификатор аэропорта назначения
         * @param yr Год выполнения рейса
         * @param qtr Квартал выполнения рейса
         * @param mth Месяц выполнения рейса
         */
        FlightInfo(airTime airTime, airportId origin, airportId dest, int yr, int qtr, int mth)
            : m_flightAirTime(airTime), m_originAirportId(origin), m_destAirportId(dest),
              m_year(yr), m_quarter(qtr), m_month(mth) {}

        /**
         * @brief Получить время в воздухе
         * @return Время в воздухе (в часах)
         */
        airTime getFlightAirTime() const { return m_flightAirTime; }

        /**
         * @brief Получить идентификатор аэропорта отправления
         * @return Идентификатор аэропорта отправления
         */
        airportId getOriginAirportId() const { return m_originAirportId; }

        /**
         * @brief Получить идентификатор аэропорта назначения
         * @return Идентификатор аэропорта назначения
         */
        airportId getDestAirportId() const { return m_destAirportId; }

        /**
         * @brief Получить год выполнения рейса
         * @return Год выполнения рейса
         */
        int getYear() const { return m_year; }

        /**
         * @brief Получить квартал выполнения рейса
         * @return Квартал выполнения рейса
         */
        int getQuarter() const { return m_quarter; }

        /**
         * @brief Получить месяц выполнения рейса
         * @return Месяц выполнения рейса
         */
        int getMonth() const { return m_month; }
    };

    using FlightGraph = std::map<airportId, std::vector<std::pair<airportId, FlightInfo>>>; /**< Граф рейсов: отображение аэропорта на список смежных аэропортов и информации о рейсах */
    using AirportsSet = std::unordered_set<airportId>; /**< Множество идентификаторов аэропортов */
    using DistanceMap = std::unordered_map<airportId, airTime>; /**< Карта расстояний от начального аэропорта */
    using PathInfo = std::pair<airportId, airTime>; /**< Информация о пути: родительский аэропорт и время перелета */
    using PathMap = std::unordered_map<airportId, PathInfo>; /**< Карта путей для восстановления маршрута */

    FlightGraph flightGraph;
    AirportsSet airports;

    /**
     * @brief Приватный конструктор по умолчанию
     *
     * Используется для создания единственного экземпляра синглтона.
     */
    FlightPathPlanner() = default;

    /**
     * @brief Парсит строку CSV с учетом кавычек
     * @param line Входная строка CSV
     * @return Вектор полей, извлеченных из строки
     */
    std::vector<std::string> parseCsvLine(const std::string& line);

    /**
     * @brief Выполняет алгоритм Дейкстры для поиска кратчайшего пути
     * @param originIndex Идентификатор аэропорта отправления
     * @return Пара, содержащая карту расстояний и карту путей
     */
    std::pair<DistanceMap, PathMap> dijkstra(airportId originIndex);

    /**
     * @brief Восстанавливает путь из результатов алгоритма Дейкстры
     * @param pathInfo Карта путей, содержащая родительские аэропорты и времена перелетов
     * @param destIndex Идентификатор аэропорта назначения
     * @return Пара векторов: список идентификаторов аэропортов и соответствующие времена перелетов
     */
    flightPath reconstructPath(const PathMap& pathInfo, airportId destIndex);

public:
    /**
     * @brief Получить единственный экземпляр класса
     * @return Ссылка на единственный экземпляр FlightPathPlanner
     */
    static FlightPathPlanner& getInstance();

    /**
     * @brief Загружает данные о рейсах из CSV-файла
     * @param filePath Путь к CSV-файлу
     * @throws FlightPathPlannerException Если файл не может быть открыт, заголовок некорректен или данные недействительны
     */
    void loadData(const std::string& filePath);

    /**
     * @brief Очищает граф рейсов и множество аэропортов
     */
    void clear();

    /**
     * @brief Проверяет, существует ли аэропорт в графе
     * @param index Идентификатор аэропорта для проверки
     * @return true, если аэропорт существует, иначе false
     */
    bool containsAirport(airportId index) const;

    /**
     * @brief Находит кратчайший путь между двумя аэропортами
     * @param originIndex Идентификатор аэропорта отправления
     * @param destIndex Идентификатор аэропорта назначения
     * @return Пара векторов: список идентификаторов аэропортов и соответствующие времена перелетов. Пусто, если путь не существует
     */
    flightPath findFlightPathBetween(airportId originIndex, airportId destIndex);

    /**
     * @brief Конструктор копирования (удален)
     *
     * Запрещает копирование экземпляра синглтона.
     */
    FlightPathPlanner(const FlightPathPlanner&) = delete;

    /**
     * @brief Оператор присваивания (удален)
     *
     * Запрещает присваивание экземпляра синглтона.
     */
    FlightPathPlanner& operator=(const FlightPathPlanner&) = delete;
};

/**
 * @brief Класс исключений для обработки ошибок в FlightPathPlanner
 *
 * Используется для уведомления о проблемах при загрузке данных, парсинге CSV
 * или других операциях с графом рейсов.
 */
class FlightPathPlannerException : public std::exception {
private:
    std::string m_errorMessage; /**< Сообщение об ошибке */

public:
    /**
     * @brief Конструктор исключения
     * @param message Сообщение об ошибке
     */
    explicit FlightPathPlannerException(const std::string& message) : m_errorMessage(message) {}

    /**
     * @brief Получить сообщение об ошибке
     * @return Указатель на строку с сообщением об ошибке
     */
    const char* what() const noexcept override { return m_errorMessage.c_str(); }
};

} // namespace fpp
