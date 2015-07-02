#include <filters/selectMode.hpp>

void selectMode(int &choose_img, int &blur, int &color, int &dilate_erode, int &edge, int &laplacian, int &hough, int &depth_dist, int &merge)
// void selectMode(int &first_filter, int &second_filter, int &third_filter, int &fourth_filter, int &fifth_filter, int &sixth_filter, int &merge)
{
	// Editor to select which filter(s) to apply
	cv::namedWindow("Filter Options", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Filter RGB or Depth", "Filter Options", &choose_img, 1);
	cv::createTrackbar("Blur Filter", "Filter Options", &blur, 1);
	cv::createTrackbar("Color Filter", "Filter Options", &color, 1);
	cv::createTrackbar("Dilate Erode Filter", "Filter Options", &dilate_erode, 1);
	cv::createTrackbar("Edge Detection Filter", "Filter Options", &edge, 1);
	cv::createTrackbar("Laplacian Filter", "Filter Options", &laplacian, 1);
	cv::createTrackbar("Hough Line Filter", "Filter Options", &hough, 1);
	cv::createTrackbar("Merge After Filtering", "Filter Options", &merge, 1);
	cv::createTrackbar("Depth Distance Calculations", "Filter Options", &depth_dist, 1);

	// cv::createTrackbar("First Filter", "Filter Options", &first_filter, 6);
	// cv::createTrackbar("Second Filter", "Filter Options", &second_filter, 6);
	// cv::createTrackbar("Third Filter", "Filter Options", &third_filter, 6);
	// cv::createTrackbar("Fourth Filter", "Filter Options", &fourth_filter, 6);
	// cv::createTrackbar("Fifth Filter", "Filter Options", &fifth_filter, 6);
	// cv::createTrackbar("Sixth Filter", "Filter Options", &sixth_filter, 6);
	// cv::createTrackbar("Merge After Filtering", "Filter Options", &`merge, 1);
}
