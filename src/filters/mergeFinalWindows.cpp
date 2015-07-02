#include "filters/mergeFinalWindows.hpp"

// 8UC3 color, 8UC1 depth
void mergeFinalWindows(cv::Mat &color, cv::Mat &color_orig, cv::Mat &depth, cv::Mat &depth_orig, cv::Mat &image, int &weight1, int &weight2, int &merge_type, int &apply, int visible)
{
	if (visible)
	{
		cv::namedWindow("Merge Final Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Merge Final Editor", &apply, 1);
		cv::createTrackbar("Weight of Color Image", "Merge Final Editor", &weight1, 100);
		cv::createTrackbar("Weight of Depth Image", "Merge Final Editor", &weight2, 100);
		cv::createTrackbar("Merge Type", "Merge Final Editor", &merge_type, 3);
	}

	else if (!visible)
	{
		cv::destroyWindow("Merge Final Editor");
	}

	if (apply)	
	{
		mergeFinal(color, color_orig, depth, depth_orig, image, weight1, weight2, merge_type);

		cv::namedWindow("Merge Final Output", cv::WINDOW_AUTOSIZE);
		cv::imshow("Merge Final Output", image);
	}

	else if (!apply)
	{
		cv::destroyWindow("Merge Final Output");
	}
}
