#ifndef ROBO_GAUSSIAN_BLUR_WINDOWS_HPP
#define ROBO_GAUSSIAN_BLUR_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboGaussianBlurWindows(cv::Mat &output, int &blur_ksize, int &sigmaX, int &sigmaY, bool visible);

#endif // ROBO_GAUSSIAN_BLUR_WINDOWS_HPP
