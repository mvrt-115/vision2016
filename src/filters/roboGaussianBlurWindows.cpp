#include "filters/roboGaussianBlurWindows.hpp"
#include "filters/RoboGaussianBlur.hpp"

void roboGaussianBlurWindows(cv::Mat &output, int &blur_ksize, int &sigmaX, int &sigmaY, bool visible)
{
	if (visible) {
		roboGaussianBlur(output, blur_ksize, sigmaX, sigmaY);
		cv::imshow("Gaussian Blur Output", output);
		cv::namedWindow("Blur Editor", cv::WINDOW_AUTOSIZE);
	} else if (!visible){
		cv::destroyWindow("Blur Editor");	
		cv::destroyWindow("Gaussian Blur Output");
	}

	cv::createTrackbar("Kernel Size", "Blur Editor", &blur_ksize, 10);
	cv::createTrackbar("Sigma X", "Blur Editor", &sigmaX, 100);
	cv::createTrackbar("Sigma Y", "Blur Editor", &sigmaY, 100);
}
