#include "filters/gaussianBlurWindows.hpp"

void gaussianBlurWindows(cv::Mat &output, int &blur_ksize, int &sigmaX, int &sigmaY, int &apply, bool visible)
{
	if (visible) // Open windows with trackbars
   	{
		cv::namedWindow("Gaussian Blur Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Gaussian Blur Editor", &apply, 1);
		cv::createTrackbar("Kernel Size", "Gaussian Blur Editor", &blur_ksize, 10);
		cv::createTrackbar("Sigma X", "Gaussian Blur Editor", &sigmaX, 100);
		cv::createTrackbar("Sigma Y", "Gaussian Blur Editor", &sigmaY, 100);
	}

   	else if (!visible) // Destroy windows
	{
		cv::destroyWindow("Gaussian Blur Editor");	
	}

	if (apply) // Apply filter (able to apply even when windows are not visible)
	{
		gaussianBlur(output, blur_ksize, sigmaX, sigmaY);

		cv::namedWindow("Gaussian Blur Output", cv::WINDOW_AUTOSIZE);
		cv::imshow("Gaussian Blur Output", output);
	}

	else if (!apply) // Destroy old output windows
	{
		cv::destroyWindow("Gaussian Blur Output");
	}
}
