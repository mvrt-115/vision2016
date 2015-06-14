#ifndef LAPACIAN
#define LAPACIAN 

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void roboLaplacian(cv::Mat& image, int ddepth, int sharpen_ksize, int scale, int delta);

#endif
