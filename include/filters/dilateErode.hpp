#ifndef DILATE_ERODE_HPP
#define DILATE_ERODE_HPP

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void dilateErode(cv::Mat& in, cv::Mat& element, int holes, int noise);

#endif // DILATE_ERODE_HPP  
