#ifndef MERGE_FINAL_WINDOWS_HPP 
#define MERGE_FINAL_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mergeFinal.hpp"

void mergeFinalWindows(cv::Mat &color, cv::Mat &color_orig, cv::Mat &depth, cv::Mat &depth_orig, cv::Mat &image, int &weight1, int &weight2, int &merge_type, int &apply, int visible);

#endif // MERGE_FINAL_WINDOWS_HPP
