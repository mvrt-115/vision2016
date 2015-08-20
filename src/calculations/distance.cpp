#include "calculations/distance.hpp"

double distance_real(cv::Mat &image, cv::Point center)
{
	cv::Scalar intensity = image.at<uchar>(center.y, center.x);
	return ( (0.1236 * tan (intensity.val[0]*4 / 2842.5 + 1.1863) * 100) / 2.54 );
	//return 0.1236 * tan (intensity.val[0] / 2842.5 + 1.1863);
}

double distance_2D(cv::Point one, cv::Point two)
{
    return sqrt(pow(one.x - two.x, 2) + pow(one.y - two.y, 2));
}

void display_distance(cv::Mat &image, cv::Point center, cv::Point orig)
{
	double distance;
	char *dist_txt = new char[50];
	
	distance = distance_real(image, center);  
	std::sprintf(dist_txt, "Distance = %.3f inches", distance);
	cv::putText(image, dist_txt, cv::Point(orig.x, orig.y), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(0, 0, 0, 0), 1, 8, false);
}
