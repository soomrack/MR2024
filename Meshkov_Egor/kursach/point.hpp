#pragma once


#include <iostream>
#include <cmath>


namespace MTL {

template <typename T>
struct Point {
    T x, y;

    Point();
    explicit Point(T x, T y);
    Point(const Point& other);

    Point& operator=(const Point& other) noexcept;
    
    bool operator==(const Point& other) const noexcept;
    bool operator!=(const Point& other) const noexcept;

    Point from_polar(double radius, double angle) noexcept;
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Point<U>& point) noexcept;
    template <typename U>
    friend std::istream& operator>>(std::istream& is, Point<U>& point) noexcept;

    double distance_to(const Point& other) const noexcept;
    double distance_to_origin() const noexcept;
    template <typename U>
    friend double distance_between(const Point<U>& p1, const Point<U>& p2) noexcept;
};


using Point2d = Point<double>;
using Point2f = Point<float>;
using Point2i = Point<int>;


}

