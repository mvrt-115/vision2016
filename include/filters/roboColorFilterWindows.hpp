#ifndef ROBO_COLOR_FILTER_WINDOWS_HPP
#define ROBO_COLOR_FILTER_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboColorFilterWindows(cv::Mat &output, int &hMin, int &hMax, int &sMin, int &sMax, int &vMin, int &vMax, bool visible);

#endif // ROBO_COLOR_FILTER_WINDOWS_HPP
