#include "filters/depthDistanceWindows.hpp"

void depthDistanceWindows(cv::Mat &image, int &center_x, int &center_y, int &orig_x, int &orig_y, int &apply, bool visible)
{
	if (visible)
	{
		cv::namedWindow("Depth Distance Editor", cv::WINDOW_AUTOSIZE);
		
		cv::createTrackbar("Apply Filter", "Depth Distance Editor", &apply, 1);
		cv::createTrackbar("Depth Point X", "Depth Distance Editor", &center_x, 640);
		cv::createTrackbar("Depth Point Y", "Depth Distance Editor", &center_y, 480);
		cv::createTrackbar("Orig X", "Depth Distance Editor", &orig_x, 640);
		cv::createTrackbar("Orig Y", "Depth Distance Editor", &orig_y, 480);
	}

	else if (!visible)
	{
		cv::destroyWindow("Depth Distance Editor");
	}

	if (apply)
	{
		cv::namedWindow("Depth Distance Output", cv::WINDOW_AUTOSIZE);
		cv::imshow("Depth Distance Output", image);
	}

	else if (!apply)
	{
		cv::destroyWindow("Depth Distance Output");
	}
}
