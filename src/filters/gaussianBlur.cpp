#include "filters/gaussianBlur.hpp"

void gaussianBlur(cv::Mat &in, int blur_ksize, int sigmaX, int sigmaY) 
{
	if ( ((blur_ksize % 2) == 0) || (blur_ksize < 0) ) // If kernel size is even or negative, round to nearest upper odd number
		blur_ksize++;

	cv::GaussianBlur( in, in, cv::Size(blur_ksize, blur_ksize), sigmaX, sigmaY, cv::BORDER_DEFAULT );
}
