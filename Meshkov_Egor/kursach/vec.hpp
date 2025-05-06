#pragma once


#include <iostream>
#include "point.hpp"


namespace MTL {


template<typename T>
class vector{
public:
    vector();
    explicit vector(T x, T y);
    vector(Point<T>);
    vector(Point<T>, Point<T>);
    vector(const vector&);
    ~vector();

    vector<T>& operator+() noexcept;
    vector<T>& operator-() noexcept;
    T operator[](size_t idx) const;

    vector& operator=(const vector& other) noexcept;
    bool operator==(const vector& other) const noexcept;
    bool operator!=(const vector& other) const noexcept;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const vector<U>& vec) noexcept;
    template <typename U>
    friend std::istream& operator>>(std::istream& is, vector<U>& vec) noexcept;

    vector operator+(const vector&) const noexcept;
    vector operator-(const vector&) const noexcept;
    vector operator*(T number) const noexcept;
    T operator*(const vector&) const noexcept;
    T operator^(const vector&) const noexcept;
    
    double norm() const noexcept;
    double angle() const noexcept;
    void rotate(T angle) noexcept;

private:
    T x, y;
};


using vector2d = vector<double>;
using vector2f = vector<float>;
using vector2i = vector<int>;


}
