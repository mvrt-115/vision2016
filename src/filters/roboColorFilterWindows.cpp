#include "filters/roboColorFilterWindows.hpp"
#include "filters/RoboColorFilter.hpp"

void roboColorFilterWindows(cv::Mat &output, int &hMin, int &hMax, int &sMin, int &sMax, int &vMin, int &vMax, bool visible)
{
	if (visible)
   	{
		roboColorFilter(output, hMin, hMax, sMin, sMax, vMin, vMax);
		
		cv::namedWindow("Color Filter Editor", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Color Filter Output", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Hue Min", "Color Filter Editor", &hMin, 255);
		cv::createTrackbar("Hue Max", "Color Filter Editor", &hMax, 255);
		cv::createTrackbar("Sat Min", "Color Filter Editor", &sMin, 255);
		cv::createTrackbar("Sat Max", "Color Filter Editor", &sMax, 255);
		cv::createTrackbar("Val Min", "Color Filter Editor", &vMin, 255);
		cv::createTrackbar("Val Max", "Color Filter Editor", &vMax, 255);
		//cv::createTrackbar("Debug Mode", "Color Filter Editor", &debugMode, 2);
		
		cv::imshow("Color Filter Output", output);
	} 
	else if (!visible)
	{
		cv::destroyWindow("Color Filter Editor");	 
		cv::destroyWindow("Color Filter Output");
	}
}
