#include "utils/distance.hpp"

// Returns the minimum distance (straight line) between two points
double distance(cv::Point one, cv::Point two)
{
    return std::sqrt(std::pow(one.x - two.x, 2) + std::pow(one.y - two.y, 2));
}

// Returns the two dimensional distance between the components of two points
double distance2D(double comp1, double comp2)
{
    return std::abs(comp2 - comp1);
}
