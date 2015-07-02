#ifndef HOUGH_LINES_HPP
#define HOUGH_LINES_HPP

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void houghLines(cv::Mat &in, int rho, int theta, int threshold, int lineMin, int maxGap);

#endif // HOUGH_LINES_HPP
