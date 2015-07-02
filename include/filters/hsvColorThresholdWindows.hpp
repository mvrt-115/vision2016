#ifndef HSV_COLOR_THRESHOLD_WINDOWS_HPP
#define HSV_COLOR_THRESHOLD_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "hsvColorThreshold.hpp"

void hsvColorThresholdWindows(cv::Mat &output, int &hMin, int &hMax, int &sMin, int &sMax, int &vMin, int &vMax, int &debugMode, int &bitAnd, int &apply, bool visible);

#endif // HSV_COLOR_THRESHOLD_WINDOWS_HPP
