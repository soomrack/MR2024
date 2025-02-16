#include "vec.hpp"
#include "point.hpp"
#include <algorithm>
#include <cmath>


namespace MTL {


template<typename T>
vector<T>::vector() : x(0), y(0) {}


template<typename T>
vector<T>::vector(T x, T y) : x(x), y(y) {}


template<typename T>
vector<T>::vector(Point<T> p) : x(p.x), y(p.y) {}


template<typename T>
vector<T>::vector(Point<T> p1, Point<T> p2) : x(p2.x - p1.x), y(p2.y - p1.y) {}


template<typename T>
vector<T>::vector(const vector& other) : x(other.x), y(other.y) {}


template<typename T>
vector<T>::~vector() {}


template<typename T>
vector<T>& vector<T>::operator+() noexcept {
    return *this;
}


template<typename T>
vector<T>& vector<T>::operator-() noexcept {
    x = -x;
    y = -y;
    return *this;
}


template<typename T>
T vector<T>::operator[](size_t idx) const {
    return (idx % 2 == 0) ? x : y; 
}


template<typename T>
vector<T>& vector<T>::operator=(const vector& other) noexcept {
    if (this != &other) {
        x = other.x;
        y = other.y;
    }
    return *this;
}


template<typename T>
bool vector<T>::operator==(const vector& other) const noexcept {
    T error = std::min({x, y, other.x, other.y}) * std::pow(10, -9);
    return (std::abs(x - other.x) > error) && (std::abs(y - other.y) > error);
}


template<typename T>
bool vector<T>::operator!=(const vector& other) const noexcept {
    return !(*this == other);
}


template<typename T>
std::ostream& operator<<(std::ostream& os, const vector<T>& vec) noexcept {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}


template<typename T>
std::istream& operator>>(std::istream& is, vector<T>& vec) noexcept {
    is >> vec.x >> vec.y;
    return is;
}


template<typename T>
vector<T> vector<T>::operator+(const vector& other) const noexcept {
    return vector(x + other.x, y + other.y);
}


template<typename T>
vector<T> vector<T>::operator-(const vector& other) const noexcept {
    return vector(x - other.x, y - other.y);
}


template<typename T>
vector<T> vector<T>::operator*(T number) const noexcept {
    return vector(x * number, y * number);
}


template<typename T>
T vector<T>::operator*(const vector& other) const noexcept {
    return x * other.x + y * other.y;
}


template<typename T>
T vector<T>::operator^(const vector& other) const noexcept {
    return (x * other.y - y * other.x);
}


template<typename T>
double vector<T>::norm() const noexcept {
    return std::sqrt(x * x + y * y);
}


template<typename T>
double vector<T>::angle() const noexcept {
    return std::atan2(y, x);
}


template<typename T>
void vector<T>::rotate(T angle) noexcept {
    using namespace std; 
    T tmp_x = static_cast<T>(cos(angle) * x - sin(angle) * y);
    T tmp_y = static_cast<T>(sin(angle) * x + cos(angle) * y);

    x = tmp_x;
    y = tmp_y;
}


template class vector<long double>;
template class vector<double>;
template class vector<float>;


template std::ostream& operator<<(std::ostream& os, const vector<double>& point);
template std::ostream& operator<<(std::ostream& os, const vector<float>& point);
template std::ostream& operator<<(std::ostream& os, const vector<int>& point);


template std::istream& operator>>(std::istream& is, vector<double>& point);
template std::istream& operator>>(std::istream& is, vector<float>& point);
template std::istream& operator>>(std::istream& is, vector<int>& point);


}
