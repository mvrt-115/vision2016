#include <filters/selectMode.hpp>

void selectMode(int &blur, int &color, int &dilate_erode, int &edge, int &laplacian, int &hough)
{
	// Editor to select which filter(s) to apply
	cv::namedWindow("Efficiency Editor", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Blur Filter", "Efficiency Editor", &blur, 1);
	cv::createTrackbar("Color Filter", "Efficiency Editor", &color, 1);
	cv::createTrackbar("Dilate Erode Filter", "Efficiency Editor", &dilate_erode, 1);
	cv::createTrackbar("Edge Filter", "Efficiency Editor", &edge, 1);
	cv::createTrackbar("Sharpen Filter", "Efficiency Editor", &laplacian, 1);
	cv::createTrackbar("Hough Line Filter", "Efficiency Editor", &hough, 1);
}
