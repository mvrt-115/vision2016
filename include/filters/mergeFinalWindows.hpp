#ifndef MERGE_FINAL_WINDOWS_HPP 
#define MERGE_FINAL_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mergeFinal.hpp"

void mergeFinalWindows(cv::Mat& color_orig, cv::Mat& final_img, int& weight1, int& weight2, int& apply, int& visible);

#endif // MERGE_FINAL_WINDOWS_HPP
