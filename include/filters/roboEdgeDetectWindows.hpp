#ifndef ROBO_EDGE_DETECT_WINDOWS_HPP
#define ROBO_EDGE_DETECT_WINDOWS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void roboEdgeDetectWindows(cv::Mat &output, int &edge_ksize, int &threshLow, int &threshHigh, bool visible);

#endif // ROBO_EDGE_DETECT_WINDOWS_HPP
