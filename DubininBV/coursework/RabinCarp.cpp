    #include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <chrono>

class RabinKarp 
{
private:
    const int base = 256;   // для ASCII
    const int module = 101;  // Большое простое число
    

    int pow(int a, int b) const 
    {
        int result = 1;
        a %= module;
        
        while (b > 0) {
            if (b & 1) {
                result = (result * a) % module;
            }
            a = (a * a) % module;
            b >>= 1;
        }
        return result;
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

    
    
    std::vector<int> search_all(const std::string& text, const std::string& pattern) const 
    {
        std::vector<int> indices;
        int n = text.length();
        int m = pattern.length();
        
        if (m == 0) {
            throw std::invalid_argument("Pattern is empty");
        }
        if (n == 0) {
            throw std::invalid_argument("Text is empty");
        }
        if (m > n) {
            throw std::invalid_argument("Pattern length longer than text length");
        }        

        int h = pow(base, m - 1);
        

        int hash_pattern = 0;
        int hash_text = 0;
        
        for (int i = 0; i < m; i++) {
            hash_pattern = (base * hash_pattern + pattern[i]) % module;
            hash_text = (base * hash_text + text[i]) % module;
        }
        

        for (int i = 0; i <= n - m; i++) {
            if (hash_pattern == hash_text) {
                bool match = true;
                for (int j = 0; j < m; j++) {
                    if (text[i + j] != pattern[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    indices.push_back(i);
                }
            }
            

            if (i < n - m) {
                hash_text = (base * (hash_text - text[i] * h) + text[i + m]) % module;
                
                if (hash_text < 0) {
                    hash_text += module;
                }
            }
        }
        
        return indices;
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
    RabinKarp rk;
    const std::string pattern = "abc";
    const std::string filename = "text.txt";
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        rk.search_in_file(filename, pattern);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Время выполнения: " << duration.count() / 1000.0 << " миллисекунд" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    return 0;
}