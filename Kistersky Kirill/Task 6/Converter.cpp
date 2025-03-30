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
    mapFile << "original_code,normalized_id\n";

    unordered_map<string, int> airportCodes;
    int currentId = 1;
    string line;
    int processedCount = 0;
    int errorCount = 0;

    // Пропускаем заголовок входного файла
    getline(inFile, line);

    while (getline(inFile, line)) {
        try {
            stringstream ss(line);
            vector<string> row;
            string cell;

            // Читаем всю строку разделенную запятыми
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            // Проверяем что строка содержит достаточно данных
            if (row.size() < 31) {
                errorCount++;
                continue;
            }

            string origin = row[23]; // ORIGIN
            string dest = row[30];   // DEST
            string distStr = row[7]; // DISTANCE

            // Удаляем кавычки если есть
            origin.erase(remove(origin.begin(), origin.end(), '"'), origin.end());
            dest.erase(remove(dest.begin(), dest.end(), '"'), dest.end());
            distStr.erase(remove(distStr.begin(), distStr.end(), '"'), distStr.end());

            // Пропускаем пустые значения
            if (origin.empty() || dest.empty() || distStr.empty()) {
                errorCount++;
                continue;
            }

            int distance = stoi(distStr);

            // Добавляем аэропорты в маппинг если их еще нет
            if (airportCodes.find(origin) == airportCodes.end()) {
                airportCodes[origin] = currentId++;
                mapFile << origin << "," << airportCodes[origin] << "\n";
            }
            if (airportCodes.find(dest) == airportCodes.end()) {
                airportCodes[dest] = currentId++;
                mapFile << dest << "," << airportCodes[dest] << "\n";
            }

            // Записываем ребро графа
            outFile << airportCodes[origin] << ","
                << airportCodes[dest] << ","
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
