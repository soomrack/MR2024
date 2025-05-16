#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <queue>
#include <algorithm>

typedef std::string origin_city_name;
typedef std::string dest_city_name;
typedef std::string city_name;
typedef std::string previous_city_name;
typedef size_t ramp_to_ramp;

const size_t inf = std::numeric_limits<size_t>::max();


class Graph
{
public:
    Graph(const size_t vertices_count);
    Graph(const std::string file_name);
    void dijkstra_alg(const std::string start);
    void get_path(std::string from, std::string to);
    void print();
private:
    size_t vertices_count;
    std::unordered_map<origin_city_name, std::unordered_map<dest_city_name, ramp_to_ramp>> vertices;
    std::unordered_map<city_name, size_t> res;
    std::unordered_map<city_name, previous_city_name> path;
    std::unordered_map<city_name, bool> complete_vert;
};

std::vector<std::string> split(const std::string &str, char delimiter);


Graph::Graph(const size_t vertices_count) : vertices_count(vertices_count)
{
    std::cout << "Создан пустой граф\n";
}


Graph::Graph(std::string file_name)
{
    std::ifstream file(file_name);
        if(!file.is_open()) {
            std::cerr << "Ошибка открытия файла" << std::endl;
        }

        std::vector<std::string> tokens;
        std::string line;
        std::string origin_airport, dest_airport; 
        size_t ramp_to_ramp;

        std::getline(file, line);
        size_t idx = 0;
        while(std::getline(file, line)) {
            tokens = split(line, ',');
            ramp_to_ramp = std::stoul(tokens[1]);

            if(ramp_to_ramp == 0) {
                continue;
            }
            
            origin_airport.insert(0, tokens[4], 1, 3);
            dest_airport.insert(0, tokens[6], 1, 3);
            
            res.insert({origin_airport, 0});
            complete_vert.insert({origin_airport, false});

            res.insert({dest_airport, 0});
            complete_vert.insert({dest_airport, false});

            vertices.insert({origin_airport, {}});
            vertices[origin_airport].insert({dest_airport, ramp_to_ramp});
            vertices[dest_airport].insert({origin_airport, ramp_to_ramp});
            idx++;
            origin_airport = "";
            dest_airport = "";
        }

        for (const auto& [v, _] : vertices) {
            res[v] = inf;
            path[v] = "no";
            complete_vert[v] = false;
        }

        vertices_count = vertices.size();

        file.close();

        std::cout << "Создан граф на основе файла\n";
}


void Graph::dijkstra_alg(const std::string start)
{
    for(auto& [key, _] : res) {
        res[key] = inf;
        complete_vert[key] = false;
        path[key] = "no";
    }

    res[start] = 0;
    path[start] = start;

    std::priority_queue<std::pair<ramp_to_ramp, city_name>, std::vector<std::pair<ramp_to_ramp, city_name>>, std::greater<std::pair<ramp_to_ramp, city_name>>> q;
    q.push({0, start});

    while (!q.empty()) {
        auto [current_distance, current_vertex] = q.top();
        q.pop();

        if(current_distance > res[current_vertex]) {
            continue;
        }

        for(auto [neighbour_vertex, distance] : vertices[current_vertex]) {
            if(res[neighbour_vertex] > res[current_vertex] + distance) {
                res[neighbour_vertex] = res[current_vertex] + distance;
                path[neighbour_vertex] = current_vertex;
                q.push({res[neighbour_vertex], neighbour_vertex});
            }
        }
    }

    const char* separator = "";
    for (const auto& [key, value] : res) {
        printf("%sAirport: %s |%ld|", separator, key.c_str(), value);
        separator = " - ";
    }
    printf("\n");

}


void Graph::get_path(std::string from, std::string to)
{
    std::vector<std::string> result;
        
        if (path.find(to) == path.end() || path[to] == "no") {
            std::cout << "Путь не существует" << std::endl;
            return; // Путь не существует
        }
        
        for (std::string current = to; current != from; current = path[current]) {
            result.push_back(current);
            if (path[current] == "nono" || current == path[current]) {
                std::cout << "Обнаружен цикл или ошибка" << std::endl;
                return; // Обнаружен цикл или ошибка
            }
        }
        result.push_back(from);
        
        std::reverse(result.begin(), result.end());
        printf("Shortest path: %ld minutes\n", res[to]);

        const char* separator = "";
        for (auto v : result) {
            printf("%s%s", separator, v.c_str());
            separator = " - ";
        }
        printf("\n");
}


void Graph::print()
{
    for(const auto& [key, value] : vertices) {
        std::cout << "Airport " << key << ":\n";
        for(const auto& [key2, value2] : value) {
            std::cout << "  Accessible airport: " << key2 << ", Time: " << value2 << '\n';
        }
    }
    std::cout << "Вершин в графе: " << vertices_count << std::endl;
}


std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);

    while(std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}


int main()
{
    std::string start = "CRW";
    std::string target = "RDU";
    Graph graph("perelety.csv");
    graph.dijkstra_alg(start);
    
    graph.get_path(start, target);
}