#include "filters/laplacianSharpenWindows.hpp"

void laplacianSharpenWindows(cv::Mat &output, int &ddepth, int &laplacian_ksize, int &scale, int &delta, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("Laplacian Sharpen Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Laplacian Sharpen Editor", &apply, 1);
		cv::createTrackbar("Kernel Size", "Laplacian Sharpen Editor", &laplacian_ksize, 9);
		cv::createTrackbar("Scale", "Laplacian Sharpen Editor", &scale, 9);
		cv::createTrackbar("Delta", "Laplacian Sharpen Editor", &delta, 9);
	}
   	else
   	{
		cv::destroyWindow("Laplacian Sharpen Editor");	 
		cv::destroyWindow("Laplacian Sharpen Output");
	}
	if (apply)
	{
		laplacianSharpen(output, ddepth, laplacian_ksize, scale, delta);
        if (visible)
        {
            cv::namedWindow("Laplacian Sharpen Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("Laplacian Sharpen Output", output);
        }
	}
	else
	{
		cv::destroyWindow("Laplacian Sharpen Output");
	}
}
