#include "filters/laplacianSharpen.hpp"

void laplacianSharpen(cv::Mat &image, int ddepth, int sharpen_ksize, int scale, int delta)
{
	if ( ((sharpen_ksize % 2) == 0) || (sharpen_ksize < 0) ) // If kernel size is even or negative, round to nearest upper odd number
		sharpen_ksize++;

	cv::Mat image_copy;
	image_copy = image.clone();

	cv::cvtColor(image_copy, image_copy, cv::COLOR_RGB2GRAY); // HSV to grayscale
 	cv::Laplacian(image_copy, image, ddepth, sharpen_ksize, scale, delta, cv::BORDER_DEFAULT);
  	cv::convertScaleAbs(image, image);
}
