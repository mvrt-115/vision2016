#include "filters/roboLaplacianWindows.hpp"
#include "filters/RoboLaplacian.hpp"

void roboLaplacianWindows(cv::Mat &output, int &ddepth, int &laplacian_ksize, int &scale, int &delta, bool visible)
{
	if (visible)
   	{
		roboLaplacian(output, ddepth, laplacian_ksize, scale, delta);

		cv::namedWindow("Laplacian Editor", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Laplacian Output", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Kernel Size", "Laplacian Editor", &laplacian_ksize, 9);
		cv::createTrackbar("Scale", "Laplacian Editor", &scale, 9);
		cv::createTrackbar("Delta", "Laplacian Editor", &delta, 9);

		cv::imshow("Laplacian Output", output);
	}
   	else if (!visible)
   	{
		cv::destroyWindow("Laplacian Editor");	 
		cv::destroyWindow("Laplacian Output");
	}
}
