#include "filters/mergeFinalWindows.hpp"

// 8UC3 color, 8UC1 depth
void mergeFinalWindows(cv::Mat& color_orig, cv::Mat& final_img, int& weight1, int& weight2, int& apply, int& visible)
{
	if (visible)
	{
		cv::namedWindow("Merge Final Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Merge Final Editor", &apply, 1);
		cv::createTrackbar("Weight of Original Image", "Merge Final Editor", &weight1, 100);
		cv::createTrackbar("Weight of Filtered Image", "Merge Final Editor", &weight2, 100);
	}
	else
	{
		cv::destroyWindow("Merge Final Editor");
		cv::destroyWindow("Merge Final Output");
	}
	if (apply)	
	{
		mergeFinal(color_orig, final_img, weight1, weight2);
        if (visible)
        {
            cv::namedWindow("Merge Final Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("Merge Final Output", final_img);
        }
	}
	else
	{
		cv::destroyWindow("Merge Final Output");
	}
}
