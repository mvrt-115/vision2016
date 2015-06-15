#include "input/input.hpp"
#include "input/freenect_cv.hpp"

Input::Input()
{ 
	std::cout << "Kinect input successfully opened." << std::endl;
}

Input::~Input()
{
	std::cout << "Kinect input successfully closed." << std::endl;
}

void Input::getBGR(cv::Mat &out)
{
    cv::Mat rgb = kinectRGB(KINECT);
    cvtColor(rgb, out, CV_RGB2BGR);
}

void Input::getIR(cv::Mat &out)
{
    kinectIR(KINECT).copyTo(out);
}

void Input::getDepth(cv::Mat &out)
{
    cv::Mat depth = kinectDepth(KINECT);
	cv::convertScaleAbs(depth, out, 0.25, 0);
}
