#ifndef ROBO_HOUGH_LINES_WINDOWS_HPP
#define ROBO_HOUGH_LINES_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboHoughLinesWindow(cv::Mat &output, int &rho, int &theta, int &threshold, int &lineMin, int &maxGap, bool visible);

#endif // ROBO_HOUGH_LINES_WINDOWS_HPP
