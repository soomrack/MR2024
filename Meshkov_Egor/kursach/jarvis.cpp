#include "point.hpp"
#include "vec.hpp"
#include "MTL.hpp"
#include <algorithm>
#include <ostream>
#include <utility>
#include <vector>


namespace MTL {


template <typename T>
static inline T rotate(const Point<T>& p, const Point<T>& q, const Point<T>& r) {
    vector<T> prev(p, q);
    vector<T> current(q, r);
    return prev ^ current;
}


template <typename T>
static inline size_t find_base_point(const std::vector<Point<T>>& points) {
size_t idx_base_point = 0;

for(size_t idx = 1; idx < points.size(); ++idx) {
    if((points[idx].x < points[0].x) 
        || (points[idx].x == points[0].x && points[idx].y < points[0].y)) {
        idx_base_point = idx;
    }
}

return idx_base_point;
}


template <typename T>
static inline void jarvis_hull_upload(std::vector<Point<T>>& hull, std::vector<Point<T>>& points) {
hull.push_back(points[0]);
points.erase(points.begin());
points.push_back(hull[0]);

bool flag_is_line = true;

while (true) {
    size_t right = 0;
    for (size_t idx = 1; idx < points.size(); ++idx) {
        T rotation = rotate(hull.back(), points[right], points[idx]);
        if (rotation < 0) { 
            right = idx;
            flag_is_line = false;
        } else if ((rotation == 0) && (distance_between(hull.back(), points[idx]) > distance_between(hull.back(), points[right]))) {
            right = idx;
        }
    }

    if (points[right] == hull[0]) {
        break;
    } else {
        hull.push_back(points[right]);
        points.erase(points.begin() + right);
    }
}

if(flag_is_line) hull.clear();

}


template <typename T>
static inline void grethem_hull_upload(std::vector<Point<T>>& hull, std::vector<Point<T>>& points) {
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    
    for(size_t idx = 2; idx < points.size(); ++idx) {
        T rotation = rotate( hull[hull.size() - 2], hull.back(), points[idx]);

        if (rotation <= 0) hull.pop_back();
        
        hull.push_back(points[idx]);
    }
    
    if(hull.size() < 3) hull.clear();
}


template <typename T>
std::vector<Point<T>> jarvis(std::vector<Point<T>> points) {
    if(points.size() < 3) return std::vector<Point<T>>();

    std::swap(points[0], points[find_base_point(points)]);
    
    std::vector<Point<T>> hull;
       
    jarvis_hull_upload(hull, points);

    return hull;
}


template <typename T>
std::vector<Point<T>> grethem(std::vector<Point<T>> points) {
    if(points.size() < 3) return std::vector<Point<T>>();

    std::swap(points[0], points[find_base_point(points)]);

    std::sort(points.begin() + 1, points.end(), [&points] (Point<T> A, Point<T> B) -> bool {
        return rotate(points[0], B, A) < 0;
    });
    
    std::vector<Point<T>> hull;

    grethem_hull_upload(hull, points);

    return hull;
}


template std::vector<Point<double>> jarvis(std::vector<Point<double>>);
template std::vector<Point<float>> jarvis(std::vector<Point<float>>);
template std::vector<Point<double>> grethem(std::vector<Point<double>>);
template std::vector<Point<float>> grethem(std::vector<Point<float>>);


}
