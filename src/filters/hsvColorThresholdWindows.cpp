#include "filters/hsvColorThresholdWindows.hpp"

void hsvColorThresholdWindows(cv::Mat &output, int &hMin, int &hMax, int &sMin, int &sMax, int &vMin, int &vMax, int &debugMode, int &bitAnd, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("HSV Color Threshold Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "HSV Color Threshold Editor", &apply, 1);
		cv::createTrackbar("Hue Min", "HSV Color Threshold Editor", &hMin, 360);
		cv::createTrackbar("Hue Max", "HSV Color Threshold Editor", &hMax, 360);
		cv::createTrackbar("Sat Min", "HSV Color Threshold Editor", &sMin, 100);
		cv::createTrackbar("Sat Max", "HSV Color Threshold Editor", &sMax, 100);
		cv::createTrackbar("Val Min", "HSV Color Threshold Editor", &vMin, 100);
		cv::createTrackbar("Val Max", "HSV Color Threshold Editor", &vMax, 100);
		cv::createTrackbar("Debug Mode", "HSV Color Threshold Editor", &debugMode, 1);
		cv::createTrackbar("BitAnd", "HSV Color Threshold Editor", &bitAnd, 1);
	} 
    else
    {
		cv::destroyWindow("HSV Color Threshold Editor");	 
		cv::destroyWindow("HSV Color Threshold Output");
        // Never enter debug mode if the editor is not visible
        debugMode = false;
    }   
	if (apply)
	{	
		hsvColorThreshold(output, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd);
        if (visible)
        {
            cv::namedWindow("HSV Color Threshold Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("HSV Color Threshold Output", output);
        }
	}
    else
	{
		cv::destroyWindow("HSV Color Threshold Output");
        debugMode = false;
		cv::destroyWindow("Hue-Filtered Debug");
        cv::destroyWindow("Saturation-Filtered Debug");
        cv::destroyWindow("Value-Filtered Debug");	
	}
}
