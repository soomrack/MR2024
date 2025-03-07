#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <limits>
 
const size_t inf = std::numeric_limits<size_t>::max();

class Graph
{
public:
    Graph(const size_t count_of_vert) : count_of_vert(count_of_vert) {
        std::cout << "Создан пустой граф\n";
    }

    Graph(const std::string file_name)
    {
        std::cout << "Создан граф на основе файла\n";

        std::ifstream file(file_name);
        if(!file.is_open()) {
            std::cerr << "Ошибка открытия файла" << std::endl;
            // throw;
        }

        size_t idx_1, idx_2, idx_3, vertex_idx, key, value;
        std::string line;

        while(std::getline(file, line)) {
            idx_1 = line.find(',');
            idx_2 = line.find(',', idx_1 + 1);
            idx_3 = line.find(',', idx_2 + 1);

            vertex_idx = static_cast<size_t>(line[idx_1 + 1]) - static_cast<size_t>('A');
            key = static_cast<size_t>(line[idx_2 + 1]) - static_cast<size_t>('A');
            value = stoul(line.substr(idx_3 + 1, line.length() - idx_3));

            vertices.insert({vertex_idx, {}});
            vertices[vertex_idx].insert({key, value});
            vertices.insert({key, {}});
            vertices[key].insert({vertex_idx, value});
        }
        count_of_vert = vertices.size();

        file.close();
    }

    void dijkstra_alg(const size_t start)
    {
        size_t res[count_of_vert];
        size_t complete_vert[count_of_vert];
        for(size_t idx = 0; idx < count_of_vert; ++idx) {
            res[idx] = inf;
            complete_vert[idx] = 1;
        }

        size_t tmp, min_idx, min;
        res[start] = 0;

        do {
            min_idx = inf;
            min = inf;

            for(size_t i = 0; i < count_of_vert; ++i) {
                if(complete_vert[i] == 1 && res[i] < min) {
                    min = res[i];
                    min_idx = i;
                }
            }

            if(min_idx != inf) {
                for(const auto& [key, value] : vertices[min_idx]) {
                    tmp = min + value;
                    if(tmp < res[key]) {
                        res[key] = tmp;
                    }
                }
                complete_vert[min_idx] = 0;
            }
        } while(min_idx < inf);

        for(size_t idx = 0; idx < count_of_vert; ++idx) {
            std::cout << res[idx] << " - ";
        }
    }

    void print()
    {
        for(const auto& [key, value] : vertices) {
            std::cout << "Вершина " << key << ":\n";
            for(const auto& [key2, value2] : value) {
                std::cout << "  Соседняя вершина: " << key2 << ", Value: " << value2 << '\n';
            }
        }
    }  
    
private:
    size_t count_of_vert;
    std::map<size_t, std::map<size_t, size_t>> vertices;
};


int main()
{
    Graph gr("air.csv");
    gr.dijkstra_alg(0);
    gr.print();
}