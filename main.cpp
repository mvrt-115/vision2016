#include "include/input/input.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

int main() {
	cv::Mat image;
	
	cv::namedWindow("BGR Feed", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Depth Feed", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("IR Feed", cv::WINDOW_AUTOSIZE);

	Input *input = new Input();

	input->getBGR(image);
	cv::imshow("BGR Feed", image);
	
	input->getDepth(image);
	cv::imshow("Depth Feed", image);
	
	input->getIR(image);
	cv::imshow("IR Feed", image);

	return 0;	
}
