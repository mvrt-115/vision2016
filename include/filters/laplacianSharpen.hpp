#ifndef LAPACIAN_SHARPEN_HPP
#define LAPACIAN_SHARPEN_HPP

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void laplacianSharpen(cv::Mat &image, int ddepth, int sharpen_ksize, int scale, int delta);

#endif // LAPLACIAN_SHARPEN_HPP
