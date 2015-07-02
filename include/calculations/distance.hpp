#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

double distance_real(cv::Mat &image, cv::Point center);
double distance_2D(cv::Point one, cv::Point two);
void display_distance(cv::Mat &image, cv::Point center, cv::Point org);

#endif // DISTANCE_HPP
