#ifndef ROBO_LAPLACIAN_WINDOWS_HPP
#define ROBO_LAPLACIAN_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboLaplacianWindows(cv::Mat &output, int &ddepth, int &laplacian_ksize, int &scale, int &delta, bool visible);

#endif // ROBO_LAPLACIAN_WINDOWS_HPP
