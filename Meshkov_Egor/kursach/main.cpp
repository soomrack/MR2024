#include "MTL.hpp"
#include "point.hpp"
#include <random>
#include <vector>

int main (int argc, char *argv[]) {
    
    using namespace MTL;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dis(0, 10);

    //std::vector<Point2d> vec = {Point2d(1, 1), Point2d(2, 2), Point2d(2, 4), Point2d(3, 4), Point2d(1, 5)};
    std::vector<Point2d> vec = {Point2d(1, 1), Point2d(2, 2), Point2d(3, 3), Point2d(4, 4), Point2d(5, 5)};
    //std::vector<Point2d> vec = {Point2d(1, 1), Point2d(2, 2), Point2d(3, 3), Point2d(1, 3)};
    //std::vector<Point2d> vec = { Point2d(1, 1), Point2d(2, 1), Point2d(3, 1), Point2d(4, 2), Point2d(5, 3), Point2d(3, 3), Point2d(2, 3), Point2d(4,4), Point2d(3, 4) };

    for(auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << (*it);
    }
    std::cout << std::endl;

    std::vector<Point2d> hull = grethem(vec); //jarvis(vec); 
    
    if(hull.empty()) std::cout << "Нет оболочки\n" << std::endl;

    for(auto it = hull.begin(); it != hull.end(); ++it) {
        std::cout << (*it);
    }
    
    return 0;
}
