#pragma once
#include "vec.hpp"
#include "point.hpp"
#include <vector>

namespace MTL {


template <typename T>
std::vector<Point<T>> jarvis(std::vector<Point<T>> points);


template <typename T>
std::vector<Point<T>> grethem(std::vector<Point<T>> points);
}
