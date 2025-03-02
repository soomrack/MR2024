#include <iostream>
#include <limits>
#include <fstream>
#include <string>

#define SIZE 7

size_t inf = std::numeric_limits<size_t>::max();

size_t** get_matrix_from_csv(std::ifstream &file)
{
    size_t** matrix = new size_t*[SIZE];
    for (size_t i = 0; i < SIZE; ++i) {
        matrix[i] = new size_t[SIZE]; // Выделяем память для столбцов
        for (size_t j = 0; j < SIZE; ++j) {
            matrix[i][j] = 0; // Инициализация нулями
        }
    }
    std::string line;
    size_t idx_1 = 0, idx_2 = 0, idx_3 = 0;
    size_t row = 0, col = 0, weight = 0;
    //std::getline(file, line);


    while(std::getline(file, line)) {
        idx_1 = line.find(',');
        idx_2 = line.find(',', idx_1 + 1);
        idx_3 = line.find(',', idx_2 + 1);
        //std::cout << line.substr(idx_1 + 1, idx_2 - idx_1 - 1) << '\t';
        //std::cout << line.substr(idx_2 + 1, idx_3 - idx_2 - 1) << '\t';
        row = static_cast<int>(line[idx_1 + 1]) - static_cast<int>('A');
        col = static_cast<int>(line[idx_2 + 1]) - static_cast<int>('A');
        weight = stoul(line.substr(idx_3 + 1, line.length() - idx_3));
        matrix[row][col] = weight;
        matrix[col][row] = weight;

        //printf("%lu %lu %lu\n", row, col, weight);
        
    }
    
    return matrix;
}

int main()
{
    std::ifstream air_flights("air.csv");
    if(!air_flights.is_open()) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        return 1;
    }
    /*
    size_t matrix[][5] ={{0, 0, 4, 0, 2},
                        {0, 0, 2, 0, 3},
                        {4, 2, 0, 5, 3},
                        {0, 0, 5, 0, 2},
                        {2, 3, 3, 2, 0}};
    */
    size_t** matrix = get_matrix_from_csv(air_flights);
    for(size_t idx = 0; idx < SIZE; ++idx) {
        for(size_t idx_2 = 0; idx_2 < SIZE; ++idx_2) {
            std::cout << matrix[idx][idx_2] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "-----------------" << '\n';
    size_t res[SIZE] = {};
    size_t complete_ver[SIZE] = {};
    for(size_t idx = 0; idx < SIZE; ++idx) {
        res[idx] = inf;
        complete_ver[idx] = 1;
    }

    size_t tmp, min_idx, min;
    size_t start = 0;
    res[start] = 0;

    do {
        min_idx = inf;
        min = inf;

        for(size_t i = 0; i < SIZE; ++i) {
            if(complete_ver[i] == 1 && res[i] < min) {
                min = res[i];
                min_idx = i;
            }
        }

        if(min_idx != inf)
        {
            for(size_t i = 0; i < SIZE; ++i) {
                if(matrix[min_idx][i] > 0) {
                    tmp = min + matrix[min_idx][i];
                    if(tmp < res[i]) {
                        res[i] = tmp;
                    }
                }
            }
            complete_ver[min_idx] = 0;
        }
    } while (min_idx < inf);

    for(size_t idx = 0; idx < SIZE; ++idx) {
        printf("%lu ", res[idx]);
    }

    air_flights.close();

    return 0;
}