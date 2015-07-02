#include "filters/hsvColorThresholdWindows.hpp"

void hsvColorThresholdWindows(cv::Mat &output, int &hMin, int &hMax, int &sMin, int &sMax, int &vMin, int &vMax, int &debugMode, int &bitAnd, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("HSV Color Threshold Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "HSV Color Threshold Editor", &apply, 1);
		cv::createTrackbar("Hue Min", "HSV Color Threshold Editor", &hMin, 255);
		cv::createTrackbar("Hue Max", "HSV Color Threshold Editor", &hMax, 255);
		cv::createTrackbar("Sat Min", "HSV Color Threshold Editor", &sMin, 255);
		cv::createTrackbar("Sat Max", "HSV Color Threshold Editor", &sMax, 255);
		cv::createTrackbar("Val Min", "HSV Color Threshold Editor", &vMin, 255);
		cv::createTrackbar("Val Max", "HSV Color Threshold Editor", &vMax, 255);
		cv::createTrackbar("Debug Mode", "HSV Color Threshold Editor", &debugMode, 1);
		cv::createTrackbar("BitAnd", "HSV Color Threshold Editor", &bitAnd, 1);
	} 
	
	else if (!visible)
	{
		cv::destroyWindow("HSV Color Threshold Editor");	 
	}
	
	if (apply)
	{	
		hsvColorThreshold(output, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd);

		cv::namedWindow("HSV Color Threshold Output", cv::WINDOW_AUTOSIZE);
		cv::imshow("HSV Color Threshold Output", output);
	}

	else if (!apply)
	{
		cv::destroyWindow("HSV Color Threshold Output");
	}
}
