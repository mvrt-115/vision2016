#include "filters/roboHoughLinesWindows.hpp"
#include "filters/RoboHoughLines.hpp"

void roboHoughLinesWindow(cv::Mat &output, int &rho, int &theta, int &threshold, int &lineMin, int &maxGap, bool visible)
{
	if (visible)
   	{
		roboHoughLines(output, rho, theta, threshold, lineMin, maxGap);

		cv::namedWindow("Hough Lines Editor", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Hough Lines Output", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Theta", "Hough Lines Editor", &theta, 180);
		cv::createTrackbar("Rho", "Hough Lines Editor", &rho, 100);
		cv::createTrackbar("Threshold", "Hough Lines Editor", &threshold, 199);
		cv::createTrackbar("LineMin", "Hough Lines Editor", &lineMin, 399);
		cv::createTrackbar("MaxGap", "Hough Lines Editor", &maxGap, 399);	

		cv::imshow("Hough Lines Output", output);
	} 
	else if (!visible)
   	{
		cv::destroyWindow("Hough Lines Editor");	 
		cv::destroyWindow("Hough Lines Output");
	}
}
