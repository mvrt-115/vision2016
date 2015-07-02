#ifndef SELECT_MODE_HPP
#define SELECT_MODE_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void selectMode(int &choose_img, int &blur, int &color, int &dilate_erode, int &edge, int &laplacian, int &hough, int &depth_dist, int &merge);
//void selectMode(int &first_filter, int &second_filter, int &third_filter, int &fourth_filter, int &fifth_filter, int &sixth_filter, int &merge);

#endif // SELECT_MODE_HPP
