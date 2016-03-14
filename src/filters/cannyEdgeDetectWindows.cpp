#include "filters/cannyEdgeDetectWindows.hpp"

void cannyEdgeDetectWindows(cv::Mat &output, int &threshLow, int &threshHigh, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("Canny Edge Detection Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Canny Edge Detection Editor", &apply, 1);
		cv::createTrackbar("Bottom Threshold", "Canny Edge Detection Editor", &threshLow, 300);
		cv::createTrackbar("Upper Threshold", "Canny Edge Detection Editor", &threshHigh, 300);
	} 
	else
   	{
		cv::destroyWindow("Canny Edge Detection Editor");	 
		cv::destroyWindow("Canny Edge Detection Output");
	}
	if (apply)	
	{
		cannyEdgeDetect(output, threshLow, threshHigh);
        if (visible)
        {
            cv::namedWindow("Canny Edge Detection Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("Canny Edge Detection Output", output);
        }
	}
	else
	{
		cv::destroyWindow("Canny Edge Detection Output");
	}
}
