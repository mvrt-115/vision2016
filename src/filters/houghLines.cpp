#include "filters/houghLines.hpp"

void houghLines(cv::Mat &image, int rho, int theta, int threshold, int lineMin, int maxGap)
{
    cv::Mat filtered = image.clone();
	std::vector <cv::Vec4i> lines;

	cv::Canny(image, filtered, 50, 200, 3); 
	
	if ( rho < 1 ) rho = 1; // Rho cannot be less than 1
	cv::HoughLinesP( filtered, lines, rho, CV_PI/theta, threshold, lineMin, maxGap );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		cv::Vec4i l = lines[i];
		cv::line(image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, CV_AA);
	}
}

