#include "filters/RoboGaussianBlur.hpp"

void roboGaussianBlur(cv::Mat& in, int blur_ksize, int sigmaX, int sigmaY) {
	if ((blur_ksize%2 != 1) && (blur_ksize > 1)) // kernel size must be odd and positive
		blur_ksize = blur_ksize*2+1;
	cv::GaussianBlur(in, in, cv::Size(blur_ksize, blur_ksize), sigmaX, sigmaY, cv::BORDER_DEFAULT);
}
