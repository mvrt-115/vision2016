#ifndef DEPTH_DISTANCE_WINDOWS_HPP
#define DEPTH_DISTANCE_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void depthDistanceWindows(cv::Mat &image, int &center_x, int &center_y, int &orig_x, int &orig_y, int &apply, bool visible);

#endif // DEPTH_DISTANCE_WINDOWS_HPP
