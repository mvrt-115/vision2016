#ifndef HOUGHLINES
#define HOUGHLINES

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

void roboHoughLines(cv::Mat& in, int rho, int theta, int threshold, int lineMin, int maxGap);

#endif
