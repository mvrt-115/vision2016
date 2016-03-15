#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>

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

const double PI = 3.14159265;
// Measurements in inches
const double TOWER_HEIGHT = 68;
const double CAMERA_HEIGHT = 7;
const int LOOPS_PER_SEC = 1; 
const int MICROSEC_TO_SEC = 1000000;
int calibDist = 110;

double calcDistance (cv::Mat& image, cv::RotatedRect& boundedRect, double focalLen, int dist, int height, int isCalib);
std::vector<cv::Point> corners (std::vector<cv::Point> pts, cv::Mat& img);
void calcHorizAngle(cv::Mat& image, double xDist, std::vector<cv::Point>& c, cv::Point2f& mc);
double calcVertAngle(cv::Mat& src, int height, int dist);
void drawBoundedRectsWindows(cv::Mat& output, double focalLen, int& d, int& h, int& calib, int& contoursThresh, int& apply, int& visible, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& shapeThresholdApply, int& shapeThresholdVisible);
void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int calib, int contoursThresh, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& applyShapeThreshold, int& shapeThreshold);
void houghCirclesWindows(cv::Mat& img, int& hcMinRadius, int& hcMaxRadius, int& threshLow, int& threshHigh, int& visible, int& apply);
void houghCircles(cv::Mat& img, int hcMinRadius, int hcMaxRadius, int& threshLow, int& threshHigh);
void shapeThreshold(cv::Mat& src, std::vector<std::vector<cv::Point> >& contour, std::vector<cv::RotatedRect>& rect, int s, int a, int minA, int maxA, int sideT, int areaT, int angleT, int& goalInd);
void shapeThresholdWindows(cv::Mat& output, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& apply, int& visible, int& goalInd);

void shapeThresholdWindows(cv::Mat& output, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& apply, int& visible, int& goalInd)
{
	if (visible)
	{
		cv::namedWindow("Shape Threshold Editor", CV_WINDOW_AUTOSIZE);
		cv::createTrackbar("Apply Filter", "Shape Threshold Editor", &apply, 1);
		cv::createTrackbar("Percent Length to Width Ratio", "Shape Threshold Editor", &sideRatio, 500);
		cv::createTrackbar("Percent Contour to Rect Area Ratio", "Shape Threshold Editor", &areaRatio, 100);
		cv::createTrackbar("Minimum Rect Area", "Shape Threshold Editor", &minArea, 30000);
		cv::createTrackbar("Maximum Rect Area", "Shape Threshold Editor", &maxArea, 30000);
		cv::createTrackbar("+- Side Threshold (Out of 100)", "Shape Threshold Editor", &sideThreshold, 100);
		cv::createTrackbar("+- Area Threshold (Out of 100)", "Shape Threshold Editor", &areaThreshold, 100);
		cv::createTrackbar("+- Angle Threshold", "Shape Threshold Editor", &angleThreshold, 100);
    }
	else
	{
		cv::destroyWindow("Shape Threshold Editor");
		cv::destroyWindow("Shape Threshold Output");
	}
	if (apply)
	{
		shapeThreshold(output, contours, rect, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, goalInd);
        if (visible)
        {
            cv::namedWindow("Shape Threshold Output", CV_WINDOW_AUTOSIZE);
            cv::imshow("Shape Threshold Output", output);
        }
	}
	else
	{
		cv::destroyWindow("Shape Threshold Output");
	}
}

void shapeThreshold(cv::Mat& src, std::vector<std::vector<cv::Point> >& contour, std::vector<cv::RotatedRect>& rect, int s, int a, int minA, int maxA, int sideT, int areaT, int angleT, int& goalInd)
{
    cv::Mat debug = src.clone();
    double sideRatio = (double) s / 100;
    double areaRatio = (double) a / 100;
    double minArea = (double) minA;
    double maxArea = (double) maxA;
    double sideThreshold = (double) sideT / 100;
    double areaThreshold = (double) areaT / 100;
    double angleThreshold = (double) angleT;

    char str[50];

	cv::Point2f vert[4];
	for (int i = 0; i < rect.size(); i++)
	{
	    rect[i].points(vert);
		double height = rect[i].size.height;
		double width = rect[i].size.width;
        double rectArea = height * width;
        double contourArea = cv::contourArea(contour[i]);

        for (int v = 0; v < 4; v++)
        {
            sprintf(str, "%i", v);
            cv::putText(debug, str, vert[v], CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
        }
        
        // If any of the following are true, the shape is not the goal
        if (std::abs((height / width) - sideRatio) > sideThreshold || 
            std::abs((contourArea / rectArea) - areaRatio) > areaThreshold || 
            rectArea < minArea || 
            rectArea > maxArea || 
            (width > height && (std::abs(rect[i].angle) > angleThreshold)) || 
            (width < height && (std::abs(std::abs(rect[i].angle) - 90) > angleThreshold)))
		{
            //sprintf(str, "H");
            //cv::putText(debug, str, cv::Point((vert[0].x + height / 2), vert[0].y), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
            //sprintf(str, "W");
            //cv::putText(debug, str, cv::Point((vert[0].x + width / 2), vert[0].y), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
            //sprintf(str, "Test: %d,%.2f,%.2f,%d", (width > height), std::abs(rect[i].angle), angleThreshold, (std::abs(rect[i].angle) > angleThreshold));
            sprintf(str,  "%d,%d,%d,%d,%.2f,%d,%d", 
                (std::abs((height / width) - sideRatio) > sideThreshold), 
                (std::abs((contourArea / rectArea) - areaRatio) > areaThreshold), 
                (rectArea < minArea),
                (rectArea > maxArea), 
                rect[i].angle,
                (width > height && (std::abs(rect[i].angle) > angleThreshold)),
                (width < height && (std::abs(std::abs(rect[i].angle) - 90) > angleThreshold)));
            cv::putText(debug, str, vert[3], CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
            // std::cerr << "Side Threshold: " << (std::abs((height / width) - sideRatio) > sideThreshold) << ", Area Threshold: " << (std::abs((contourArea / rectArea) - areaRatio) > areaThreshold) << ", < Min Area: " << (rectArea < minArea) << ", > Max Area: " << (rectArea > maxArea) << ", Angle: " << rect[i].angle << ", Angle Threshold: " << (std::abs(rect[i].angle) > angleThreshold) << ", Angle Threshold: " << (width < height && (std::abs(std::abs(rect[i].angle) - 90) > angleThreshold)) << "\n";
            //sprintf(str, "-(%i)", i);
            //cv::putText(debug, str, cv::Point(contour[i][0]), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
            contour.erase(contour.begin() + i);
			rect.erase(rect.begin() + i);
			i--; //because vector just got smaller
		}
        else
        {
            goalInd = i;
            //std::cerr << "Ratio (" << i << "): " << (height / width) << ", area: " << rectArea << ", area ratio : " << (contourArea / rectArea) << ", angle: " << rect[i].angle << "\n";
            sprintf(str, "+(%i)", i);
            cv::putText(debug, str, cv::Point(contour[i][0]), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
        }
        
        //cv::imshow("Debug Shape Threshold", debug);
	}
}

void drawBoundedRectsWindows(cv::Mat& output, double focalLen, int& d, int& h, int& calib, int& contoursThresh, int& apply, int& visible, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& shapeThresholdApply, int& shapeThresholdVisible)
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
	else
	{
		cv::destroyWindow("Distance and Shooting Angles Editor");
		cv::destroyWindow("Distance and Shooting Angles Output");
	}
	if (apply)
	{
		drawBoundedRects(output, focalLen, d, h, calib, contoursThresh, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, shapeThresholdApply, shapeThresholdVisible);
        if (visible)
        {
            cv::namedWindow("Distance and Shooting Angles Output", CV_WINDOW_AUTOSIZE);
            cv::imshow("Distance and Shooting Angles Output", output);
        }
	}
	else
	{
		cv::destroyWindow("Distance and Shooting Angles Output");
	}
}

void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int calib, int contoursThresh, int& sideRatio, int& areaRatio, int& minArea, int& maxArea, int& sideThreshold, int& areaThreshold, int& angleThreshold, int& applyShapeThreshold, int& shapeThreshold)
{
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	// Convert src to gray format
	cv::cvtColor(src, src, CV_BGR2GRAY);
	// Detect edges using Threshold
	cv::threshold(src, src, contoursThresh, 255, cv::THRESH_BINARY);
	cv::findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	src = cv::Mat::zeros(src.size(), CV_8UC3);

	// Get the minimally sized bounded rectangles
	std::vector<cv::RotatedRect> minRect( contours.size());
	for(int i = 0; i < contours.size(); i++)
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));

	std::vector<cv::Point> c;
    int goalInd = 0;
    double dist = 0;
	// Bounded rectangle is the one at the 0th index
	if (minRect.size() > 0)
	{
		shapeThresholdWindows(src, contours, minRect, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, applyShapeThreshold, shapeThreshold, goalInd);
	}
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Moments mu = cv::moments(contours[i], false);
        cv::circle(src, cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00), 5, cv::Scalar(255, 255, 0));
        // Check if the center of mass is inside the contour region and if it is, it is not the goal
        double dist = cv::pointPolygonTest(contours[i], cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00), true);
        if (dist > -5 || dist < -15)
        {
            contours.erase(contours.begin() + i);
            minRect.erase(minRect.begin() + i);
			i--; //because vector just got smaller
        }
    }
	if (contours.size() > 0)
	{
        //std::cerr << "Contours Size: " << contours.size() << "\n";
        cv::Moments mu = moments(contours[0], false);
        cv::Point2f mc = cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
		c = corners(contours[goalInd], src);
		dist = calcDistance(src, minRect[0], focalLen, d, h, calib);
		calcHorizAngle(src, d, c, mc);
        calcVertAngle(src, h, dist);
	}
	for(int i = 0; i < contours.size(); i++)
	{
		cv::Point2f rect_points[4]; 
		minRect[i].points(rect_points);
		cv::Scalar color = cv::Scalar(0, 255, 0);
		cv::drawContours(src, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
		color = cv::Scalar(0, 255, 255);
		// Draw the bounded rectangle
		for(int j = 0; j < 4; j++)
		{
			cv::line(src, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
		}
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

double calcVertAngle(cv::Mat& src, int height, int dist)
{
    char str[50];
    double theta = ((static_cast<double>(height) / dist) * 180 / PI);

	sprintf(str, "Vert Angle   = %4.2f", theta);
	cv::putText(src, str, cv::Point(10, 470), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

}

void calcHorizAngle(cv::Mat& image, double xDist, std::vector<cv::Point>& c, cv::Point2f& mc)
{
	double wPixel = distance2D(c[0].x, c[3].x);
	double center = image.cols / 2;
	double distFromCenter = mc.x - center;
	double inchesPerPixel = 20 / wPixel;

	double yDist = distFromCenter * inchesPerPixel;
	double theta = std::asin(yDist / xDist) * 180 / PI;

	cv::circle(image, mc, 5, cv::Scalar(255, 255, 0));
	cv::line(image, cv::Point(center, mc.y), mc, cv::Scalar (255, 255, 100));

	char str[50];
	sprintf(str, "Horiz Angle  = %4.2f", theta);
	cv::putText(image, str, cv::Point(10, 430), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);
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

	char str[50];
	
	//sprintf(str, "Line Length  = %4.2f", wPixel);
	//cv::putText(image, str, cv::Point(10, 400), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Focal Length = %4.2f", focalLen);
	cv::putText(image, str, cv::Point(10, 410), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Distance     = %4.2f", d);
	cv::putText(image, str, cv::Point(10, 450), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	return d;
}

void houghCirclesWindows(cv::Mat& img, int& hcMinRadius, int& hcMaxRadius, int& threshLow, int& threshHigh, int& visible, int& apply)
{
	if (visible)
	{
		cv::namedWindow("Hough Circles Editor", cv::WINDOW_AUTOSIZE);	
		cv::createTrackbar("Apply Filter", "Hough Circles Editor", &apply, 1);
		cv::createTrackbar("Min Radius", "Hough Circles Editor", &hcMaxRadius, 500);
		cv::createTrackbar("Max Radius", "Hough Circles Editor", &hcMinRadius, 500);
	}
	else
	{
		cv::destroyWindow("Hough Circles Editor");
		cv::destroyWindow("Hough Circles Output");
	}
	if (apply)
	{
		houghCircles(img, hcMinRadius, hcMaxRadius, threshLow, threshHigh);
        if (visible)
        {
            cv::namedWindow("Hough Circles Output", CV_WINDOW_AUTOSIZE);
            cv::imshow("Hough Circles Output", img);
        }
	}
	else
	{
		cv::destroyWindow("Hough Circles Output");
	}
}

void houghCircles(cv::Mat& img, int hcMinRadius, int hcMaxRadius, int& threshLow, int& threshHigh)
{    
  	std::vector<cv::Vec3f> circles;

	if (enumCvType(img) == "CV_8UC3")
		Canny(img, img, threshLow, threshHigh);
 	// Apply the Hough Transform to find the circles
 	cv::HoughCircles( img, circles, CV_HOUGH_GRADIENT, 2, img.rows/8, hcMaxRadius, hcMinRadius);
	cvtColor(img, img, CV_GRAY2BGR);

  	// Draw the circles detected
  	for( size_t i = 0; i < circles.size(); i++ )
  	{
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        cv::circle(img, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        cv::circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
   	}
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
	int boundedRects = 0;
	int houghCircles = 0;
    int shapeThreshold = 0;

	// Parameters for applying filters even if windows are closed
	int apply_blur = 1;
	int apply_color = 1;
	int apply_dilate_erode = 0;
	int apply_edge = 0;
	int apply_laplacian = 0;
	int apply_hough = 0;
	int apply_depth_dist = 0;
	int apply_merge = 0;
	int applyBoundedRects = 1;
	int applyHoughCircles = 0;
    int applyShapeThreshold = 1;

	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 10;
	int sigmaY = 10;

	// hsvColorThreshold parameters
	int hMin = 90;
	int hMax = 180;
	int sMin = 0;
	int sMax = 100;
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
	int threshHigh = 245;
	
	// laplaci
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

	// houghCircles parameters
	int hcMinRadius = 116;
	int hcMaxRadius = 212;	

	// mergeFinal parameters
	int weight1 = 100;
	int weight2 = 100;

	// drawBoundedRects parameters
	bool isAuton = false;
	double focalLen = 675.0;
	// Tower height is 7 feet 1 inches which is 85 inches
	int height = TOWER_HEIGHT - CAMERA_HEIGHT;
	int calibStatus = 0;
	int contoursThresh = 140;

    // Shape threshold parameters
    int s = 150;
    int a = 35;
    int minA = 800;
    int maxA = 25000;
    int sideT = 100;
    int areaT = 20;
    int angleT = 40;

	std::cerr << "\n";
    std::cerr << " =========== FILTER LIST =========== " << "\n";
    std::cerr << "|                                   |" << "\n";
    std::cerr << "| (0) No Filter                     |" << "\n";
    std::cerr << "| (1) Gaussian Blur Filter          |" << "\n";
    std::cerr << "| (2) HSV Color Filter              |" << "\n";
    std::cerr << "| (3) Dilate and Erode Filter       |" << "\n";
    std::cerr << "| (4) Canny Edge Detection Filter   |" << "\n";
    std::cerr << "| (5) Laplacian Sharpen Filter      |" << "\n";
    std::cerr << "| (6) Hough Lines Filter            |" << "\n";
    std::cerr << "| (7) Merge Final Outputs           |" << "\n";
    std::cerr << "| (8) Distance and Shooting Angles  |" << "\n";
    std::cerr << "|                                   |" << "\n";
    std::cerr << " =================================== " << "\n";
    std::cerr << "\n";

    std::cerr << "\n";
    std::cerr << " ============== NOTICE ============= " << "\n";
    std::cerr << "|                                   |" << "\n";
    std::cerr << "| Press 'q' to quit without saving  |" << "\n";
    std::cerr << "| Press ' ' to pause                |" << "\n";
    std::cerr << "|                                   |" << "\n";
    std::cerr << " =================================== " << "\n";
    std::cerr << "\n";

	int port = 0;
	cv::VideoCapture camera;
	do
	{
		std::cerr << "Enter the port number of the camera (-1 to quit): ";
		std::cin >> port;

		// Reprompt if user enters invalid input
		if (port <= 10 && port >= 0)
		{
			camera = cv::VideoCapture (port);
			if(!camera.isOpened())
			{
				std::cerr << "\nUnable to open camera at Port " << port << "\n\n";
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
		selectMode(blur, color, dilate_erode, edge, laplacian, hough, shapeThreshold, merge, boundedRects);

		// Use images
		if (argc > 2)
		{
			rgb = cv::imread(argv[1]);

			// No data
			if (!rgb.data)
			{
				//std::cerr << "No image data" << std::endl;`
				return -1;
			}
		}
		else 
		{
			camera >> rgb;
		}
		rgb.copyTo(image);
		//cv::imshow("BGR Feed", rgb);

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
		cannyEdgeDetectWindows(image, threshLow, threshHigh, apply_edge, edge);
		laplacianSharpenWindows(image, ddepth, laplacian_ksize, scale, delta, apply_laplacian, laplacian);
		houghLinesWindows(image, rho, theta, threshold, lineMin, maxGap, apply_hough, hough);
		houghCirclesWindows(image, hcMinRadius, hcMaxRadius, threshLow, threshHigh, houghCircles, applyHoughCircles);
		drawBoundedRectsWindows(image, focalLen, calibDist, height, calibStatus, contoursThresh, applyBoundedRects, boundedRects, s, a, minA, maxA, sideT, areaT, angleT, applyShapeThreshold, shapeThreshold);
		mergeFinalWindows(rgb, image, weight1, weight2, apply_merge, merge);
		kill = cv:: waitKey(5);
		
	}
	return 0;	
}


