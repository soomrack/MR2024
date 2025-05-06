#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

const string path = "flights.csv";
const string path_out = "filtred_flights.csv";

const vector<string> filter = {"ORIGIN", "RAMP_TO_RAMP", "DEST_CITY_NAME"};

vector<string> split(const string& str, char delimiter) {
    vector<string> words;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != string::npos) {
        words.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    words.push_back(str.substr(start));
    return words;
}

vector<int> get_filtered_cols(const string& first_str) {
    vector<string> first_str_array = split(first_str, ',');
    vector<int> data;

    for (const string& need_word : filter) {
        auto it = find(first_str_array.begin(), first_str_array.end(), need_word);
        if (it != first_str_array.end()) {
            int index = distance(first_str_array.begin(), it);
            data.push_back(index);
        }
    }

    return data;
}

string joinWithComma(const vector<string>& words) {
    if (words.empty()) {
        return "";
    }

    ostringstream oss;
    oss << words[0];

    for (size_t i = 1; i < words.size(); ++i) {
        oss << "," << words[i];
    }

    return oss.str();
}

vector<string> filterWordsByIndices(const vector<string>& words, const vector<int>& indices) {
    vector<string> result;
    
    for (int index : indices) {
        if (index >= 0 && static_cast<size_t>(index) < words.size()) {
            result.push_back(words[index]);
        }
    }
    
    return result;
}

int main() {
    ifstream fin(path);
    ofstream fout(path_out);

    if (!fin.is_open() || !fout.is_open()) {
        cerr << "Error with opening file!" << endl;
        return 1;
    }

    cout << "File is opened!" << endl;

    string line;
    int k = 0;
    vector<int> needed_cols;

    while (getline(fin, line)) {
        if (k == 0) {
            needed_cols = get_filtered_cols(line);
            fout << joinWithComma(filter) << endl;
            k++;
            continue;
        }

        vector<string> separated_words = split(line, ',');
        vector<string> filtered_words = filterWordsByIndices(separated_words, needed_cols);
        if(filtered_words[1] == "0.00") {
            continue;
        }
        fout << joinWithComma(filtered_words) << endl;
    }

    fin.close();
    fout.close();

    cout << "Filtering completed successfully!" << endl;
    return 0;
}