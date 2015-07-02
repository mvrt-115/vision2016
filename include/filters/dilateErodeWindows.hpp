#ifndef DILATE_ERODE_WINDOWS_HPP
#define DILATE_ERODE_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dilateErode.hpp"

void dilateErodeWindows(cv::Mat &output, cv::Mat &element, int &holes, int &noise, int &apply, bool visible);

#endif // DILATE_ERODE_WINDOWS_HPP
