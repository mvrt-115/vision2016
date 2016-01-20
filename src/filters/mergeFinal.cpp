#include "filters/mergeFinal.hpp"
#include "logging/enumCvType.hpp"

void mergeFinal(cv::Mat& color_orig, cv::Mat& final_img, int weight1, int weight2)
{
	if (enumCvType(color_orig) == "CV_8UC1") 
	{
		cv::Mat* channels = new cv::Mat [3];
	
		// Make color_orig into CV_8UC3
		channels[0] = color_orig;
		channels[1] = channels[0];
		channels[2] = channels[0];
		cv::merge(channels, 3, color_orig);
		delete [] channels;
	}
    cv::addWeighted(color_orig, (double) weight1 / 100, final_img, (double) weight2 / 100, 3, final_img); // Original color + filtered
}
