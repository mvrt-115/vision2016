#include "filters/dilateErodeWindows.hpp"

void dilateErodeWindows(cv::Mat &output, cv::Mat &element, int &holes, int &noise, int &apply, bool visible)
{
	if (visible)
   	{
		cv::namedWindow("Dilate and Erode Editor", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Apply Filter", "Dilate and Erode Editor", &apply, 1);
		cv::createTrackbar("Noise Iterations", "Dilate and Erode Editor", &noise, 15);
		cv::createTrackbar("Hole Iterations", "Dilate and Erode Editor", &holes, 15);
	}
   	else
   	{
		cv::destroyWindow("Dilate and Erode Editor");	 
		cv::destroyWindow("Dilate and Erode Output");
	}
	if (apply)
	{
		dilateErode(output, element, holes, noise);
        if (visible)
        {
            cv::namedWindow("Dilate and Erode Output", cv::WINDOW_AUTOSIZE);
            cv::imshow("Dilate and Erode Output", output);
        }
	}
	else
	{
		cv::destroyWindow("Dilate and Erode Output");
	}
}
