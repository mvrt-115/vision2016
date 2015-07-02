#ifndef HOUGH_LINES_WINDOWS_HPP
#define HOUGH_LINES_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "houghLines.hpp"

void houghLinesWindows(cv::Mat &output, int &rho, int &theta, int &threshold, int &lineMin, int &maxGap, int &apply, bool visible);

#endif // HOUGH_LINES_WINDOWS_HPP
