#include "filters/cannyEdgeDetect.hpp"

void cannyEdgeDetect(cv::Mat& image, int threshLow, int threshHigh)
{
    cv::Mat channels[3];
	
	cv::split(image, channels);
	cv::Canny(image, channels[0], threshLow, threshHigh);
		
		channels[1] = channels[0];
		channels[2] = channels[0];	
	
	cv::merge(channels, 3, image);
}
