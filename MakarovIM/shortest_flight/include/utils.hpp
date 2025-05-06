#pragma once

#include <string>
#include <algorithm>

class StringUtils {
public:
	static std::string normalize_city_name(const std::string& raw_city_name);
};