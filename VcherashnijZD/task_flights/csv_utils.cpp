#include "csv_utils.h"
#include <vector>
#include <string>

std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> elements = {""};
    unsigned int element_cnt = 0;
    bool is_quoted = false;

    for (char ch : str) {
        switch (ch) {
        case ',':
            if (!is_quoted) {
                element_cnt++;
                elements.push_back("");
            }
            break;
        case '"':
            is_quoted = !is_quoted;
            break;
        default:
            elements[element_cnt].push_back(ch);
            break;
        }
    }

    return elements;
}

