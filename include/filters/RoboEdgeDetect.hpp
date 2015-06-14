#ifndef EDGEDETECT
#define EDGEDETECT

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void roboEdgeDetect(cv::Mat& image, int edge_ksize, int threshLow, int threshHigh);

#endif
