#ifndef CANNY_EDGE_DETECT_HPP
#define CANNY_EDGE_DETECT_HPP

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void cannyEdgeDetect(cv::Mat& image, int threshLow, int threshHigh);

#endif // CANNY_EDGE_DETECT_HPP
