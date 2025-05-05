#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <limits>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <map>

struct Edge
{
    std::string dest;
    double distance;
};

std::unordered_map<std::string, std::vector<Edge>> buildGraph(const std::vector<std::vector<std::string>> &data)
{
    std::unordered_map<std::string, std::vector<Edge>> graph;

    for (size_t i = 0; i < data.size(); ++i)
    {
        const auto &columns = data[i];
        std::string origin = columns[22];
        std::string dest = columns[33];
        std::string distanceStr = columns[9];
        std::string passengersStr = columns[4];

        // Проверяем, что passengersStr не пустая
        if (passengersStr.empty())
        {
            continue;
        }

        double distance = std::stod(distanceStr);
        int passengers = std::stoi(passengersStr);

        // Проверяем, что дистанция не равна 0
        if (distance == 0)
        {
            continue;
        }

        // Проверяем, что рейс не грузовой (количество пассажиров не равно 0)
        if (passengers == 0)
        {
            continue;
        }

        // Добавляем ребро в граф
        graph[origin].push_back({dest, distance});
    }

    return graph;
}

std::vector<std::vector<std::string>> ReadCSV(const std::string &file_name)
{
    std::ifstream file(file_name);

    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл: " << file_name << std::endl;
        return {};
    }

    std::vector<std::vector<std::string>> data;
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        bool in_qq = false;

        while (std::getline(ss, cell, ','))
        {
            if (in_qq) // внутри кавычек
            {
                row.back() += "," + cell;
                if (cell.back() == '"')
                {
                    row.back().pop_back(); // Убираем вторую кавычку
                    in_qq = false;
                }
            }
            else
            {
                if (cell.front() == '"')
                {
                    cell.erase(0, 1); // Убираем первую кавычку
                    in_qq = true;
                }
                row.push_back(cell);
            }
        }

        data.push_back(row);
    }
    file.close();
    return data;
}

// Алгоритм Дейкстры
std::pair<std::map<std::string, double>, std::map<std::string, std::string>> dijkstra(
    const std::unordered_map<std::string, std::vector<Edge>> &graph,
    const std::string &start)
{
    std::map<std::string, double> distances;         // Расстояния до аэропортов
    std::map<std::string, std::string> predecessors; // Предшественники
    std::priority_queue<std::pair<double, std::string>, std::vector<std::pair<double, std::string>>, std::greater<>> pq;

    // Инициализация
    for (const auto &node : graph)
    {
        distances[node.first] = std::numeric_limits<double>::infinity(); // node.first — это ключ (название аэропорта, строка std::string)
    }
    distances[start] = 0;
    pq.push({0, start});

    while (!pq.empty())
    {
        std::string current = pq.top().second;
        pq.pop();

        for (const Edge &edge : graph.at(current)) // graph.at(current) возвращает std::vector<Edge>, это ссылка к дистанции через ключ
        {
            std::string neighbor = edge.dest;
            double newDistance = distances[current] + edge.distance; // distances[current]: Это текущее кратчайшее расстояние от начальной вершины (start) до вершины current, хранящееся в distances.

            if (newDistance < distances[neighbor])
            {
                distances[neighbor] = newDistance;
                predecessors[neighbor] = current;
                pq.push({newDistance, neighbor});
            }
        }
    }

    return {distances, predecessors};
}

// Восстановление пути
std::vector<std::string> getPath(
    const std::map<std::string, std::string> &predecessors,
    const std::string &start,
    const std::string &end)
{
    std::vector<std::string> path;
    std::string current = end;

    while (current != start)
    {
        path.push_back(current);
        current = predecessors.at(current);
    }
    path.push_back(start);

    std::reverse(path.begin(), path.end()); // Переворачиваем путь
    return path;
}

int main()
{
    std::string file_name = "data.csv";
    std::string start, end;

    auto data = ReadCSV(file_name);

    auto graph = buildGraph(data);

    std::cout << "Введите начальный аэропорт: ";
    std::cin >> start;
    std::cout << "Введите конечный аэропорт: ";
    std::cin >> end;

    // Поиск кратчайшего пути
    auto [distances, predecessors] = dijkstra(graph, start);

    if (distances.find(end) == distances.end() || distances[end] == std::numeric_limits<double>::infinity())
    {
        std::cout << "Путь из " << start << " в " << end << " не найден." << std::endl;
    }
    else
    {
        std::vector<std::string> path = getPath(predecessors, start, end);
        std::cout << "Кратчайший путь из " << start << " в " << end << ": ";
        for (size_t idx = 0; idx < path.size(); ++idx)
        {
            std::cout << path[idx];
            if (idx < path.size() - 1)
                std::cout << " -> ";
        }
        std::cout << "\nРасстояние: " << distances[end] << std::endl;
    }

    return 0;
}

// В дланных есть ковычки
