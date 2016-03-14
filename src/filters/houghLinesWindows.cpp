#include "filters/houghLinesWindows.hpp"

void houghLinesWindows(cv::Mat &output, int &rho, int &theta, int &threshold, int &lineMin, int &maxGap, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("Hough Lines Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Hough Lines Editor", &apply, 1);
		cv::createTrackbar("Theta", "Hough Lines Editor", &theta, 180);
		cv::createTrackbar("Rho", "Hough Lines Editor", &rho, 100);
		cv::createTrackbar("Threshold", "Hough Lines Editor", &threshold, 200);
		cv::createTrackbar("LineMin", "Hough Lines Editor", &lineMin, 400);
		cv::createTrackbar("MaxGap", "Hough Lines Editor", &maxGap, 400);
	}
   	else
	{
		cv::destroyWindow("Hough Lines Editor");	
		cv::destroyWindow("Hough Lines Output");
	}
	if (apply)
	{
		houghLines(output, rho, theta, threshold, lineMin, maxGap);
        if (visible)
        {
            cv::namedWindow("Hough Lines Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("Hough Lines Output", output);
        }
	}	
	else
	{
		cv::destroyWindow("Hough Lines Output");
	}
}
