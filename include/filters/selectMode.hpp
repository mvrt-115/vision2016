#ifndef SELECT_MODE_HPP
#define SELECT_MODE_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void selectMode(int &blur, int &color, int &dilate_erode, int &edge, int &laplacian, int &hough, int &depth_dist, int &merge, int& boundedRects);

#endif // SELECT_MODE_HPP
