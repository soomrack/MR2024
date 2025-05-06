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


const size_t inf = std::numeric_limits<size_t>::max();


class Graph
{
public:
    Graph(const size_t vertices_count);
    Graph(const std::string file_name);
    void dijkstra_alg(const size_t start);
    void get_path(size_t from, size_t to);
    void print();
private:
    size_t vertices_count;
    std::unordered_map<size_t, std::unordered_map<size_t, size_t>> vertices;
    std::unordered_map<size_t, size_t> res;
    std::unordered_map<size_t, size_t> path;
    std::unordered_map<size_t, bool> complete_vert;
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
        size_t origin_airport, dest_airport, ramp_to_ramp;

        std::getline(file, line);
        size_t idx = 0;
        while(std::getline(file, line)) {
            tokens = split(line, ',');
            ramp_to_ramp = std::stoul(tokens[1]);

            if(ramp_to_ramp == 0) {
                continue;
            }
            
            origin_airport = std::stoul(tokens[3]);
            dest_airport = std::stoul(tokens[5]);
            
            res.insert({origin_airport, 0});
            complete_vert.insert({origin_airport, false});

            vertices.insert({origin_airport, {}});
            vertices[origin_airport].insert({dest_airport, ramp_to_ramp});
            vertices[dest_airport].insert({origin_airport, ramp_to_ramp});
            idx++;
        }

        for (const auto& [v, _] : vertices) {
            res[v] = inf;
            path[v] = inf;
            complete_vert[v] = false;
        }

        vertices_count = vertices.size();

        file.close();

        std::cout << "Создан граф на основе файла\n";
}


void Graph::dijkstra_alg(const size_t start)
{
    for(auto& [key, _] : res) {
        res[key] = inf;
        complete_vert[key] = false;
        path[key] = inf;
    }

    res[start] = 0;
    path[start] = start;

    std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<std::pair<size_t, size_t>>> q;
    q.push({0, start});

    while (!q.empty()) {
        auto [cur_d, v] = q.top();
        q.pop();

        if(cur_d > res[v]) {
            continue;
        }

        for(auto [u, w] : vertices[v]) {
            if(res[u] > res[v] + w) {
                res[u] = res[v] + w;
                path[u] = v;
                q.push({res[u], u});
            }
        }
    }

    const char* separator = "";
    for (const auto& [key, value] : res) {
        printf("%sAirport ID: %ld |%ld|", separator, key, value);
        separator = " - ";
    }
    printf("\n");
}


void Graph::get_path(size_t from, size_t to)
{
    std::vector<size_t> result;
        
        if (path.find(to) == path.end() || path[to] == inf) {
            return; // Путь не существует
        }
        
        for (size_t current = to; current != from; current = path[current]) {
            result.push_back(current);
            if (path[current] == inf || current == path[current]) {
                return; // Обнаружен цикл или ошибка
            }
        }
        result.push_back(from);
        
        std::reverse(result.begin(), result.end());
        printf("Shortest path: %ld minutes\n", res[to]);

        const char* separator = "";
        for (auto v : result) {
            printf("%s%ld", separator, v);
            separator = " - ";
        }
        printf("\n");
}


void Graph::print()
{
    for(const auto& [key, value] : vertices) {
        std::cout << "Airport ID " << key << ":\n";
        for(const auto& [key2, value2] : value) {
            std::cout << "  Accessible airport ID: " << key2 << ", Time: " << value2 << '\n';
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
    size_t start = 10001;
    size_t target = 10012;
    Graph graph("perelety.csv");
    graph.dijkstra_alg(start);

    
    graph.get_path(start, target);
}