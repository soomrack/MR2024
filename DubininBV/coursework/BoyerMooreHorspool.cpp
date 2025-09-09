#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <chrono>

class BoyerMooreHorspool 
{
private:
    std::unordered_map<char, int> build_shift_table(const std::string& pattern) 
    {
        std::unordered_map<char, int> shift_table;
        int pattern_length = pattern.length();
        

        if (pattern_length == 0) {
            throw std::invalid_argument("Pattern cannot be empty");
        }
        
        for (int i = 0; i < pattern_length - 1; i++) {
            shift_table[pattern[i]] = pattern_length - 1 - i;
        }

        shift_table[pattern[pattern_length - 1]] = pattern_length;
        
        return shift_table;
    }



    std::string read_file_content(const std::string& filename) 
    {
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file");
        }
        
        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();

        if (content.empty()) {
            throw std::runtime_error("File is empty");
        }
        
        return content;
    }



    void print_occurrences(const std::vector<int>& occurrences) 
    {
        std::cout << "Found " << occurrences.size() << " occurrences at positions: ";
        for (int pos : occurrences) {
            std::cout << pos << " ";
        }
        std::cout << "\n";
    }

    

    std::vector<int> search_all(const std::string& text, const std::string& pattern) 
    {
        std::vector<int> occurrences;
        int text_length = text.length();
        int pattern_length = pattern.length();
        

        if (pattern_length == 0) {
            throw std::invalid_argument("Pattern is empty");
        }
        if (text_length == 0) {
            throw std::invalid_argument("Text is empty");
        }
        if (pattern_length > text_length) {
            throw std::invalid_argument("Pattern length longer than text length");
        }

        std::unordered_map<char, int> shift_table = build_shift_table(pattern);
        
        int i = 0;
        while (i <= text_length - pattern_length) {
            int j = pattern_length - 1; 


            while (j >= 0 && text[i + j] == pattern[j]) {
                j--;
            }

            if (j < 0) {
                occurrences.push_back(i);
                i++;
            } else {
                char mismatch_char = text[i + pattern_length - 1];
                
                int shift = pattern_length;
                if (shift_table.find(mismatch_char) != shift_table.end()) {
                    shift = shift_table[mismatch_char];
                }
                
                i += shift;
            }
        }

        return occurrences;
    }

public:
    void search_in_file(const std::string& filename, const std::string& pattern) 
    {
        std::string text = read_file_content(filename);
        print_occurrences(search_all(text, pattern));
    }
};



int main() 
{
    BoyerMooreHorspool bmh;
    const std::string pattern = "abc";
    const std::string filename = "text.txt";

    try {
        auto start = std::chrono::high_resolution_clock::now();
        bmh.search_in_file(filename, pattern);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Время выполнения: " << duration.count() / 1000.0 << " миллисекунд" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}