#include "filters/roboDilateErodeWindows.hpp"
#include "filters/RoboDilateErode.hpp"

void roboDilateErodeWindows(cv::Mat &output, cv::Mat &element, int &holes, int &noise, bool visible)
{
	if (visible)
   	{
		roboDilateErode(output, element, holes, noise);

		cv::namedWindow("Dilate and Erode Editor", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Dilate and Erode Output", cv::WINDOW_AUTOSIZE);

		cv::createTrackbar("Noise Iterations", "Dilate and Erode Editor", &noise, 15);
		// cv::createTrackbar("Filter Size", "Dilate and Erode Editor", &size, 10);
		cv::createTrackbar("Hole Iterations", "Dilate and Erode Editor", &holes, 15);

		cv::imshow("Dilate and Erode Output", output);
	}
   	else if (!visible)
   	{
		cv::destroyWindow("Dilate and Erode Editor");	 
		cv::destroyWindow("Dilate and Erode Output");
	}
}
