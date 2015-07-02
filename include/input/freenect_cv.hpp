#ifndef FREENECT_CV_HPP
#define FREENECT_CV_HPP

#include <opencv2/imgproc/imgproc.hpp>

#include <stdexcept>
#ifdef __linux__
#include <libfreenect_sync.h>
#endif
#ifdef __APPLE__
#include <libfreenect/libfreenect_sync.h>
#endif

cv::Mat kinectDepth(int index);
cv::Mat kinectRGB(int index);
cv::Mat kinectIR(int index);

#endif // FREENECT_CV_HPP
