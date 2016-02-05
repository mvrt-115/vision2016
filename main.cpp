#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>

#include "include/logging/enumCvType.hpp"

#include "include/utils/distance.hpp"

#include "include/filters/selectMode.hpp"
#include "include/filters/gaussianBlurWindows.hpp"
#include "include/filters/hsvColorThresholdWindows.hpp"
#include "include/filters/dilateErodeWindows.hpp"
#include "include/filters/cannyEdgeDetectWindows.hpp"
#include "include/filters/laplacianSharpenWindows.hpp"
#include "include/filters/houghLinesWindows.hpp"
#include "include/filters/mergeFinalWindows.hpp"
#include "include/filters/depthDistanceWindows.hpp"

double calcDistance (cv::Mat& image, cv::RotatedRect& boundedRect, double focalLen, int dist, int height, int isCalib);
std::vector<cv::Point> corners (std::vector<cv::Point> pts, cv::Mat& img);
void calcHorizAngle(cv::Mat& image, double xDist, std::vector<cv::Point>& c, std::vector<cv::Point2f>& mc);
void drawBoundedRectsWindows(cv::Mat output, double focalLen, int& d, int& h, int& calib, int& contoursThresh, int& apply, int& visible);
void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int calib, int contoursThresh);

// double getDistance();
// double getVerticalAngle();
// double getHorizontalAngle();
// void findContours(cv::Mat& src, std::vector<std::vector<cv::Point>>& contours, int thresh);
// void findBoundedRects(cv::Mat& src, std::vector<std::vector<cv::Point>>& contours, );

void rectThreshold(std::vector<std::vector<cv::Point> >& contour, cv::RotatedRect& rect, double dist)
{
	const double lScale = 0.417;
	const double wScale = 0.244697;
	cv::Point2f vertices[4];
	rect.points(vertices);
	double rotatedArea = distance(vertices[0],vertices[1])*distance(vertices[1], vertices[2]);
	for (int i = 0; i < contour.size(); i++)
	{
		double contourArea = cv::contourArea(contour[i]);
		double length = distance(vertices[0], vertices[3]); 
		double width =  distance(vertices[0], vertices[1]);
		if (length < width)
			std::swap(length, width);
		double lRatio = length/dist;a
		double wRatio = width/dist;
		std::cout << "distance: " << dist << ", lRatio: " << lRatio << ", wRatio: " << wRatio << "\n";
		if (!(std::abs(lRatio - lScale) < 0.1 && std::abs(wRatio - wScale) < 0.1))
		{
			contour.erase(contour.begin() + i);
			i--; //because contour just got smaller
			std::cout << "contour erased..." << "\n";
		}
	}
}

void drawBoundedRectsWindows(cv::Mat output, double focalLen, int& d, int& h, int& calib, int& contoursThresh, int& apply, int& visible)
{
	if (visible)
	{
		cv::namedWindow("Distance and Shooting Angles Editor", CV_WINDOW_AUTOSIZE);
		cv::createTrackbar("Apply Filter", "Distance and Shooting Angles Editor", &apply, 1);
		cv::createTrackbar("Calibration Status", "Distance and Shooting Angles Editor", &calib, 1);
		cv::createTrackbar("Calibration Distance", "Distance and Shooting Angles Editor", &d, 300);
		cv::createTrackbar("Calibration Height", "Distance and Shooting Angles Editor", &h, 200);
		cv::createTrackbar("Contours Threshold", "Distance and Shooting Angles Editor", &contoursThresh, 200);
	}
	else if (!visible)
	{
		cv::destroyWindow("Distance and Shooting Angles Editor");
	}

	if (apply)
	{
		drawBoundedRects(output, focalLen, d, h, calib, contoursThresh);

		cv::namedWindow("Distance and Shooting Angles Output", CV_WINDOW_AUTOSIZE);
		cv::imshow("Distance and Shooting Angles Output", output);
	}
	else if (!apply)
	{
		cv::destroyWindow("Distance and Shooting Angles Output");
	}
}

void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int calib, int contoursThresh)
{
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	// Convert src to gray format
	cv::cvtColor(src, src, CV_BGR2GRAY);
	// Detect edges using Threshold
	cv::threshold(src, src, contoursThresh, 255, cv::THRESH_BINARY);
	cv::findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	src = cv::Mat::zeros(src.size(), CV_8UC3);

	// Get the moments
	std::vector<cv::Moments> mu(contours.size());
	for(int i = 0; i < contours.size(); i++)
			mu[i] = moments(contours[i], false);

	//  Get the mass centers
	std::vector<cv::Point2f> mc(contours.size());
	for(int i = 0; i < contours.size(); i++)
			mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

	// Get the minimally sized bounded rectangles
	std::vector<cv::RotatedRect> minRect( contours.size());
	for(int i = 0; i < contours.size(); i++)
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));

	// Extract the four corners of the bounded rectangles
	for(int i = 0; i < contours.size(); i++)
	{
		cv::Point2f rect_points[4]; 
		minRect[i].points(rect_points);
	}
	std::vector<cv::Point> c;
	// Bounded rectangle is the one at the 0th index
	if (minRect.size() > 0)
	{
		double dist = calcDistance(src, minRect[0], focalLen, d, h, calib);
		rectThreshold(contours, minRect[0], dist);
	}
	if (contours.size() > 0)
	{
		c = corners(contours[0], src);
		calcHorizAngle(src, d, c, mc);
	}

	/*
	// Draw the mass centers
	for (int i = 0; i < mc.size(); i++)
	{
	cv::circle(src, mc[i], 5, cv::Scalar(255, 255, 0));
	}
	// Draw the corners of the contour object
	for (int i = 0; i < c.size(); i++)
	{
	cv::circle(src, c[i], 5, cv::Scalar(0, 255, 0));
	}
	*/
	// Draw the contours
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 255, 0);
		cv::drawContours(src, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
		color = cv::Scalar(0, 255, 255);
		/*
		// Draw the bounded rectangle
		for(int j = 0; j < 4; j++)
		{
			cv::line(src, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
		}
		*/
	}
}

std::vector<cv::Point> corners (std::vector<cv::Point> pts, cv::Mat& img)
{
	cv::Point tLeft = cv::Point (0, 0);
	cv::Point tRight = cv::Point (img.cols, 0);
	cv::Point bLeft = cv::Point (0, img.rows);
	cv::Point bRight = cv::Point (img.cols, img.rows);

	// Initialize to the maximum possible values
	double tLeftD = img.cols;
	double tRightD = img.cols;
	double bLeftD = img.cols;
	double bRightD = img.cols;

	std::vector<cv::Point> c;
	c.push_back(bLeft);
	c.push_back(tLeft);
	c.push_back(tRight);
	c.push_back(bRight);

	for (int i = 0; i < pts.size(); i++)
	{
		if (distance(pts[i], bLeft) < bLeftD)
		{
			bLeftD = distance(pts[i], bLeft);
			c.at(0) = pts[i];
		}

		if (distance(pts[i], tLeft) < tLeftD)
		{
			tLeftD = distance(pts[i], tLeft);
			c.at(1) = pts[i];
		}

		if (distance(pts[i], tRight) < tRightD)
		{
			tRightD = distance(pts[i], tRight);
			c.at(2) = pts[i];
		}

		if (distance(pts[i], bRight) < bRightD)
		{
			bRightD = distance(pts[i], bRight);
			c.at(3) = pts[i];
		}
	}
	return c;
}

void calcHorizAngle(cv::Mat& image, double xDist, std::vector<cv::Point>& c, std::vector<cv::Point2f>& mc)
{
	const double PI = 3.14159265;
	double wPixel = distance2D(c[0].x, c[3].x);
	double center = image.cols / 2;
	double distFromCenter = mc[0].x - center;
	double inchesPerPixel = 20 / wPixel;

	double yDist = distFromCenter * inchesPerPixel;
	double theta = std::asin(yDist / xDist) * 180 / PI;

	cv::circle(image, mc[0], 5, cv::Scalar(255, 255, 0));
	cv::line(image, cv::Point(center, mc[0].y), mc[0], cv::Scalar (255, 255, 100));

	char str[50];
	sprintf(str, "Horiz Angle  = %4.2f", theta);
	cv::putText(image, str, cv::Point(10, 440), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);
}

double calcDistance (cv::Mat& image, cv::RotatedRect& boundedRect, double focalLen, int dist, int height, int isCalib)
{
	// 20 inches real width
	int wReal = 20;
	int wPixel = 0;
	double theta = 0;
	double d = 0;
	const double PI = 3.14159265;

	cv::Point2f vert[4];
	boundedRect.points(vert);
	int length = static_cast<int>(distance (vert[0], vert[3]));
	int width = static_cast<int>(distance (vert[0], vert[1]));

	if (length > width)
	{
		cv::line(image, vert[0], vert[3], cv::Scalar(100, 0, 100));
		cv::line(image, vert[1], vert[2], cv::Scalar(100, 0, 100));
		wPixel = length;
	}
	// Object is sideways
	else
	{
		cv::line(image, vert[0], vert[1], cv::Scalar(100, 0, 100));
		cv::line(image, vert[2], vert[3], cv::Scalar(100, 0, 100));
		wPixel = width;
	}

	if (isCalib == 1)
		focalLen = (static_cast<double>(wPixel * dist)) / wReal;
	else if (isCalib == 0)
		d = (wReal * focalLen) / wPixel;

	theta = asin(static_cast<double>(height) / dist) * 180 / PI;

	char str[50];
	
	//sprintf(str, "Line Length  = %4.2f", wPixel);
	//cv::putText(image, str, cv::Point(10, 400), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Focal Length = %4.2f", focalLen);
	cv::putText(image, str, cv::Point(10, 420), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Distance	 = %4.2f", d);
	cv::putText(image, str, cv::Point(10, 460), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Vert Angle   = %4.2f", theta);
	cv::putText(image, str, cv::Point(10, 480), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	//return dist;
	return d;
}

int main( int argc, char *argv[])
{
	// Parameters for selecting which filters to apply
	int blur = 0;
	int color = 0;
	int dilate_erode = 0;
	int edge = 0;
	int laplacian = 0;
	int hough = 0;
	int depth_dist = 0;
	int merge = 0;
	int boundedRects = 1;

	// Parameters for applying filters even if windows are closed
	int apply_blur = 1;
	int apply_color = 1;
	int apply_dilate_erode = 0;
	int apply_edge = 1;
	int apply_laplacian = 0;
	int apply_hough = 0;
	int apply_depth_dist = 0;
	int apply_merge = 1;
	int applyBoundedRects = 1;

	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 10;
	int sigmaY = 10;

	// hsvColorThreshold parameters
	int hMin = 100;
	int hMax = 180;
	int sMin = 0;
	int sMax = 30;
	int vMin = 80;
	int vMax = 100;
	int debugMode = 0;
		// 0 is none, 1 is bitAnd between h, s, and v
	int bitAnd = 1;

	// dilateErode parameters
	int holes = 0;
	int noise = 0;
	int size = 1;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
					  cv::Size(2 * size + 1, 2 * size + 1), 
					  cv::Point(size, size) );
	
	// cannyEdgeDetect parameters
	int threshLow = 100;
	int threshHigh = 300;
	
	// laplacianSharpen parameters
	int laplacian_ksize = 3;
		// optional scale value added to image
	int scale = 1;
		// optional delta value added to image
	int delta = 0;
	int ddepth = CV_16S;
	
	// houghLines parameters
	int rho = 1;
	int theta = 180;
	int threshold = 50;
	int lineMin = 50;
	int maxGap = 10;

	// mergeFinal parameters
	int weight1 = 100;
	int weight2 = 100;

	// drawBoundedRects parameters
	bool isAuton = false;
	double focalLenClose = 675.0;
	double focalLenFar = 675.0;
	double cameraHeight = 0.933;
	const double TOWER_HEIGHT = 85;
	int dist = 180;
	// Tower height is 7 feet 1 inches which is 85 inches
	int height = TOWER_HEIGHT - cameraHeight;
	int calibStatus = 0;
	int contoursThresh = 140;

	std::cout << "\n";
	std::cout << " =========== FILTER LIST =========== " << "\n";
	std::cout << "|								   |" << "\n";
	std::cout << "| (0) No Filter					 |" << "\n";
	std::cout << "| (1) Gaussian Blur Filter		  |" << "\n";
	std::cout << "| (2) HSV Color Filter			  |" << "\n";
	std::cout << "| (3) Dilate and Erode Filter	   |" << "\n";
	std::cout << "| (4) Canny Edge Detection Filter   |" << "\n";
	std::cout << "| (5) Laplacian Sharpen Filter	  |" << "\n";
	std::cout << "| (6) Hough Lines Filter			|" << "\n";
	std::cout << "| (7) Merge Final Outputs		   |" << "\n";
	std::cout << "| (8) Distance and Shooting Angles  |" << "\n";
	std::cout << "|								   |" << "\n";
	std::cout << " =================================== " << "\n";
	std::cout << "\n";

	std::cout << "\n";
	std::cout << " ============== NOTICE ============= " << "\n";
	std::cout << "|								   |" << "\n";
	std::cout << "| Press 'q' to quit without saving  |" << "\n";
	std::cout << "| Press ' ' to pause				|" << "\n";
	std::cout << "|								   |" << "\n";
	std::cout << " =================================== " << "\n";
	std::cout << "\n";

	int port = 0;
	cv::VideoCapture camera;
	do
	{
		std::cout << "Enter the port number of the camera (-1 to quit): ";
		std::cin >> port;

		// Reprompt if user enters invalid input
		if (port <= 10 && port >= 0)
		{
			camera = cv::VideoCapture (port);
			if(!camera.isOpened())
			{
				std::cout << "\nUnable to open camera at Port " << port << "\n\n";
			}
		}
	}
	while (port != -1 && !camera.isOpened());
	if (port == -1) return -1;

	// Matrices for holding image data
	cv::Mat rgb, rgb_orig;
	cv::Mat image;

		// Press q to quit the program
	char kill = ' ';
	while ((kill != 'q') && (kill != 's'))
	{
		// Press space to pause program, then any key to resume
		if (kill == ' ')
		{
			cv::waitKey(0);
		}
		selectMode(blur, color, dilate_erode, edge, laplacian, hough, depth_dist, merge, boundedRects);

		// Use images
		if (argc > 2)
		{
			rgb = cv::imread(argv[1]);

			// No data
			if (!rgb.data)
			{
				std::cout << "No image data" << std::endl;
				return -1;
			}
		}
		else 
		{
				camera >> rgb;
		}
		cv::imshow("BGR Feed", rgb);
		rgb.copyTo(image);

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
		if (isAuton)
			drawBoundedRectsWindows(image, focalLenFar, dist, height, calibStatus, contoursThresh, applyBoundedRects, boundedRects);
		else
			drawBoundedRectsWindows(image, focalLenClose, dist, height, calibStatus, contoursThresh, applyBoundedRects, boundedRects);
		cannyEdgeDetectWindows(image, threshLow, threshHigh, apply_edge, edge);
		laplacianSharpenWindows(image, ddepth, laplacian_ksize, scale, delta, apply_laplacian, laplacian);
		houghLinesWindows(image, rho, theta, threshold, lineMin, maxGap, apply_hough, hough);
		mergeFinalWindows(rgb, image, weight1, weight2, apply_merge, merge);
		kill = cv::waitKey(5);
	}
	return 0;	
}
