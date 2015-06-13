#ifndef COLORFILTER
#define COLORFILTER

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

void roboColorFilter(cv::Mat& in, int hMin = 0, int hMax = 255, int sMin = 0, int sMax = 255, int vMin = 0, int vMax = 255, bool bitAnd = true);

#endif
