#ifndef GAUSSIAN_BLUR_WINDOWS_HPP
#define GAUSSIAN_BLUR_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "gaussianBlur.hpp"

void gaussianBlurWindows(cv::Mat &output, int &blur_ksize, int &sigmaX, int &sigmaY, int &apply, bool visible);

#endif // GAUSSIAN_BLUR_WINDOWS_HPP
