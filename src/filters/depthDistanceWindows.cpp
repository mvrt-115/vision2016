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
		cv::Point center (center_x, center_y); // Point to calculate distance of
		cv::Point orig (orig_x, orig_y); // Point to place text (bottom left corner of text)

		display_distance(image, center, orig);
		cv::circle(image, center, 1, cv::Scalar(0, 0, 255), -1, 8, 0); // Draw a filled in red circle at the point to calculate distance of
		cv::circle(image, center, 30, cv::Scalar(0, 0, 255), 1, 8, 0); // Draw a second filled in red circle around the point to calculate distance of

		cv::namedWindow("Depth Distance Output", cv::WINDOW_AUTOSIZE);
		cv::imshow("Depth Distance Output", image);
	}

	else if (!apply)
	{
		cv::destroyWindow("Depth Distance Output");
	}
}
