#ifndef DILATEERODE
#define DILATEERODE

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

void roboDilateErode(cv::Mat& in, cv::Mat& element, int holes, int noise);

#endif
