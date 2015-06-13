#ifndef INPUT_HPP
#define INPUT_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
#include <iostream>

enum CameraType {
	KINECT = 0
};


class Input
{
public:
    std::shared_ptr<cv::VideoCapture> cap;
	Input();
	~Input();
	void getBGR(cv::Mat &out);
    void getIR(cv::Mat &out);
    void getDepth(cv::Mat &out);
};

#endif // INPUT_HPP
