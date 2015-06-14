#ifndef ROBO_DILATE_ERODE_WINDOWS_HPP
#define ROBO_DILATE_ERODE_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboDilateErodeWindows(cv::Mat &output, cv::Mat &element, int &holes, int &noise, bool visible);

#endif // ROBO_DILATE_ERODE_WINDOWS_HPP
