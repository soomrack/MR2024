#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <map>

using AirportCode = std::string;
using AirportId = int;

struct Node {
    float min_time;
    bool processed;
    AirportId prev_node;
};

#endif // COMMON_H

