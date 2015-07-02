#ifndef LAPLACIAN_SHARPEN_WINDOWS_HPP
#define LAPLACIAN_SHARPEN_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "laplacianSharpen.hpp"

void laplacianSharpenWindows(cv::Mat &output, int &ddepth, int &laplacian_ksize, int &scale, int &delta, int &apply, bool visible);

#endif // LAPLACIAN_SHARPEN_WINDOWS_HPP
