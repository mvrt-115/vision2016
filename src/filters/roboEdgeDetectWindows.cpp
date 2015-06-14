#include "filters/roboEdgeDetectWindows.hpp"
#include "filters/RoboEdgeDetect.hpp"

void roboEdgeDetectWindows(cv::Mat &output, int &edge_ksize, int &threshLow, int &threshHigh, bool visible)
{
	if (visible)
   	{
		roboEdgeDetect(output, edge_ksize, threshLow, threshHigh);

		cv::namedWindow("Edge Detection Editor", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Edge Detection Output", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Kernel Size", "Edge Detection Editor", &edge_ksize, 7);
		cv::createTrackbar("Bottom Threshold", "Edge Detection Editor", &threshLow, 255);
		cv::createTrackbar("Upper Threshold", "Edge Detection Editor", &threshHigh, 255);

		cv::imshow("Edge Detection Output", output);
	} 
	else if (!visible)
   	{
		cv::destroyWindow("Edge Detection Editor");	 
		cv::destroyWindow("Edge Detection Output");
	}
}
