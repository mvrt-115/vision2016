#ifndef HSV_COLOR_THRESHOLD_HPP
#define HSV_COLOR_THRESHOLD_HPP

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void hsvColorThreshold(cv::Mat &in, int hMin, int hMax, int sMin, int sMax, int vMin, int vMax, bool DEBUG, bool bitAnd);

#endif // HSV_COLOR_THRESHOLD_HPP 
