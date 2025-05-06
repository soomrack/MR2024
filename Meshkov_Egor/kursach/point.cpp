#include "point.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ostream>


namespace MTL {


template <typename T>
Point<T>::Point() : x(0), y(0) {}


template <typename T>
Point<T>::Point(T x, T y) : x(x), y(y) {}


template <typename T>
Point<T>::Point(const Point& other) : x(other.x), y(other.y) {}


template <typename T>
Point<T>& Point<T>::operator=(const Point& other) noexcept{
    if (this != &other) {
        x = other.x;
        y = other.y;
    }
    return *this;
}


template <typename T>
bool Point<T>::operator==(const Point& other) const noexcept {
    using std::abs;
    T error_x = std::min(abs(x), abs(other.x)) * 1e-9;
    T error_y = std::min(abs(y), abs(other.y)) * 1e-9; 
    return (abs(x - other.x) < error_x) && (abs(y - other.y) < error_y);
}


template <>
bool Point<int>::operator==(const Point& other) const noexcept {
    return x == other.x && y == other.y;
}


template <typename T>
bool Point<T>::operator!=(const Point& other) const noexcept {
    return !(*this == other);
}


template <typename U>
std::ostream& operator<<(std::ostream& os, const Point<U>& point) noexcept{
    os << "(" << point.x << ", " << point.y << ")" << std::endl;
    return os;
}


template <typename U>
std::istream& operator>>(std::istream& is, Point<U>& point) noexcept {
    is >> point.x >> point.y;
    return is;
}


template <typename T>
double Point<T>::distance_to(const Point& other) const noexcept {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}


template <typename T>
double Point<T>::distance_to_origin() const noexcept {
    return distance_to(Point(0, 0));
}


template <typename T>
Point<T> Point<T>::from_polar(double radius, double angle) noexcept {
    double rad = angle * M_PI / 180.0;
    return Point(radius * std::cos(rad), radius * std::sin(rad));
}


template <>
Point<int> Point<int>::from_polar(double radius, double angle) noexcept {
    double rad = angle * M_PI / 180.0;
    double x = radius * std::cos(rad);
    double y = radius * std::sin(rad);
    return Point(static_cast<int>(std::round(x)), static_cast<int>(std::round(y))); // Округляем и возвращаем точку
}



template <typename U>
double distance_between(const Point<U>& p1, const Point<U>& p2) noexcept {
    return p1.distance_to(p2);
}


template class Point<double>;
template class Point<float>;
template class Point<int>;


template double distance_between(const Point<double>& p1, const Point<double>& p2) noexcept;
template double distance_between(const Point<float>& p1, const Point<float>& p2) noexcept;
template double distance_between(const Point<int>& p1, const Point<int>& p2) noexcept;


template std::ostream& operator<<(std::ostream& os, const Point<double>& point);
template std::ostream& operator<<(std::ostream& os, const Point<float>& point);
template std::ostream& operator<<(std::ostream& os, const Point<int>& point);


template std::istream& operator>>(std::istream& is, Point<double>& point);
template std::istream& operator>>(std::istream& is, Point<float>& point);
template std::istream& operator>>(std::istream& is, Point<int>& point);


}
