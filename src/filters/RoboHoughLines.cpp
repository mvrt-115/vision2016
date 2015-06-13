#include "filters/RoboHoughLines.hpp"

void roboHoughLines(cv::Mat& in, int rho, int theta, int threshold, int lineMin, int maxGap)
{
    cv::Mat filtered = in.clone();
	std::vector <cv::Vec4i> lines;
	cv::Canny(filtered, filtered, 50, 200, 3); 
	cv::HoughLinesP(filtered, lines, rho+1, CV_PI/theta, threshold+1, lineMin+1, maxGap+1 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
		cv::Vec4i l = lines[i];
		cv::line(in, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
	}
}

