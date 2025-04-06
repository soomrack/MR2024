#include "utils.hpp"
#include <algorithm>
#include <cctype>

std::string StringUtils::normalize_city_name(const std::string& name) {
    std::string normalized = name;

    // Удаление кавычек
    normalized.erase(std::remove(normalized.begin(), normalized.end(), '"'), normalized.end());

    // Приведение к нижнему регистру
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](char c) { return std::tolower(c); });

    // Удаление пробелов в начале и в конце
    auto start = normalized.find_first_not_of(" \t");
    auto end = normalized.find_last_not_of(" \t");

    if (start == std::string::npos) return "";
    return normalized.substr(start, end - start + 1);
}
