#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

string getInputFilename() {
    string filename;
    while (true) {
        cout << "Enter input CSV filename (or drag file here): ";

        if (cin.peek() == '\n') {
            clearInputBuffer();
        }
        getline(cin, filename);

        // Удаляем кавычки если файл перетащен в окно терминала
        filename.erase(remove(filename.begin(), filename.end(), '"'), filename.end());

        // Удаляем возможные пробелы в начале и конце
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);

        if (filename.empty()) {
            cout << "Filename cannot be empty. Please try again.\n";
            continue;
        }

        if (!fileExists(filename)) {
            cout << "File '" << filename << "' not found. Please try again.\n";
            continue;
        }

        return filename;
    }
}

void convertAirlineData(const string& inputFile, const string& outputFile, const string& mappingFile) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);
    ofstream mapFile(mappingFile);

    if (!inFile.is_open()) {
        cerr << "Error: Could not open input file " << inputFile << endl;
        return;
    }
    if (!outFile.is_open()) {
        cerr << "Error: Could not create output file " << outputFile << endl;
        return;
    }
    if (!mapFile.is_open()) {
        cerr << "Error: Could not create mapping file " << mappingFile << endl;
        return;
    }

    // Заголовки выходных файлов
    outFile << "from,to,distance\n";
    mapFile << "original_code,normalized_id,city_state\n";  // Добавили city_state

    unordered_map<string, pair<int, string>> airportCodes;  // code -> (id, city_state)
    int currentId = 1;
    string line;
    int processedCount = 0;
    int errorCount = 0;

    // Читаем заголовок входного файла
    getline(inFile, line);
    vector<string> headers;
    stringstream headerStream(line);
    string header;

    while (getline(headerStream, header, ',')) {
        headers.push_back(header);
    }

    // Находим индексы нужных столбцов
    int originIdx = -1, destIdx = -1, distIdx = -1, originCityIdx = -1;
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == "ORIGIN") originIdx = i;
        else if (headers[i] == "DEST") destIdx = i;
        else if (headers[i] == "DISTANCE") distIdx = i;
        else if (headers[i] == "ORIGIN_CITY_NAME") originCityIdx = i;
    }

    if (originIdx == -1 || destIdx == -1 || distIdx == -1) {
        cerr << "Error: Required columns not found in input file" << endl;
        return;
    }

    while (getline(inFile, line)) {
        try {
            vector<string> row;
            bool inQuotes = false;
            string currentCell;

            // Парсим строку с учетом кавычек
            for (char c : line) {
                if (c == '"') {
                    inQuotes = !inQuotes;
                }
                else if (c == ',' && !inQuotes) {
                    row.push_back(currentCell);
                    currentCell.clear();
                }
                else {
                    currentCell += c;
                }
            }
            row.push_back(currentCell);

            // Проверяем что строка содержит достаточно данных
            if (row.size() <= max(originIdx, max(destIdx, distIdx))) {
                errorCount++;
                continue;
            }

            string origin = row[originIdx];
            string dest = row[destIdx];
            string distStr = row[distIdx];
            string cityState = originCityIdx != -1 ? row[originCityIdx] : "Unknown";

            // Удаляем кавычки если они есть
            auto removeQuotes = [](string& s) {
                s.erase(remove(s.begin(), s.end(), '"'), s.end());
                };

            removeQuotes(origin);
            removeQuotes(dest);
            removeQuotes(distStr);
            removeQuotes(cityState);

            // Пропускаем пустые значения
            if (origin.empty() || dest.empty() || distStr.empty()) {
                errorCount++;
                continue;
            }

            int distance = stoi(distStr);

            // Добавляем аэропорты в маппинг если их еще нет
            if (airportCodes.find(origin) == airportCodes.end()) {
                airportCodes[origin] = { currentId, cityState };
                mapFile << origin << "," << currentId << "," << cityState << "\n";
                currentId++;
            }
            if (airportCodes.find(dest) == airportCodes.end()) {
                // Для аэропорта назначения используем ту же cityState (или можно добавить отдельную колонку)
                airportCodes[dest] = { currentId, cityState };
                mapFile << dest << "," << currentId << "," << cityState << "\n";
                currentId++;
            }

            // Записываем ребро графа
            outFile << airportCodes[origin].first << ","
                << airportCodes[dest].first << ","
                << distance << "\n";

            processedCount++;
            if (processedCount % 10000 == 0) {
                cout << "Processed " << processedCount << " records..." << endl;
            }

        }
        catch (...) {
            errorCount++;
            continue;
        }
    }

    cout << "\nConversion complete!\n"
        << "- Output graph: " << outputFile << "\n"
        << "- Airport mapping: " << mappingFile << "\n"
        << "- Total airports: " << airportCodes.size() << "\n"
        << "- Total routes processed: " << processedCount << "\n"
        << "- Errors encountered: " << errorCount << endl;
}


int main() {
    cout << "=== Airline Data Converter ===" << endl;

    string inputFile = getInputFilename();
    string outputFile = "airline_graph.csv";
    string mappingFile = "airport_mapping.csv";

    convertAirlineData(inputFile, outputFile, mappingFile);

    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}
