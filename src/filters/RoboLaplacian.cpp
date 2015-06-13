#include "filters/RoboLaplacian.hpp"

void roboLaplacian(cv::Mat& image, int ddepth, int sharpen_ksize, int scale, int delta)
{
	cv::Mat channels[3];

	cv::cvtColor(image, image, cv::COLOR_RGB2GRAY); // HSV to grayscale
 	cv::Laplacian(image, image, ddepth, sharpen_ksize, scale, delta, cv::BORDER_DEFAULT);
  	cv::convertScaleAbs(image, image);
	channels[0] = image;
	channels[1] = image;
	channels[2] = image;
	cv::merge(channels, 3, image);
}
