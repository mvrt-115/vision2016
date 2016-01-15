#include <filters/selectMode.hpp>

void selectMode(int &blur, int &color, int &dilate_erode, int &edge, int &laplacian, int &hough, int &depth_dist, int &merge)
{
	// Editor to select which filter(s) to apply
	cv::namedWindow("Filter Options", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Blur Filter", "Filter Options", &blur, 1);
	cv::createTrackbar("Color Filter", "Filter Options", &color, 1);
	cv::createTrackbar("Dilate Erode Filter", "Filter Options", &dilate_erode, 1);
	cv::createTrackbar("Edge Detection Filter", "Filter Options", &edge, 1);
	cv::createTrackbar("Laplacian Filter", "Filter Options", &laplacian, 1);
	cv::createTrackbar("Hough Line Filter", "Filter Options", &hough, 1);
	cv::createTrackbar("Merge After Filtering", "Filter Options", &merge, 1);
	cv::createTrackbar("Depth Distance Calculations", "Filter Options", &depth_dist, 1);
}
