#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>

#include "include/logging/enumCvType.hpp"

#include "include/filters/selectMode.hpp"
#include "include/filters/gaussianBlurWindows.hpp"
#include "include/filters/hsvColorThresholdWindows.hpp"
#include "include/filters/dilateErodeWindows.hpp"
#include "include/filters/cannyEdgeDetectWindows.hpp"
#include "include/filters/laplacianSharpenWindows.hpp"
#include "include/filters/houghLinesWindows.hpp"
#include "include/filters/mergeFinalWindows.hpp"
#include "include/filters/depthDistanceWindows.hpp"

double calculateDistance (cv::Mat& image, cv::RotatedRect& boundedRect);
double distance(cv::Point one, cv::Point two);
std::vector<cv::Point> corners (std::vector<cv::Point> pts, cv::Mat& img);
void calcHorizAngle(cv::Mat& image, double xDist, std::vector<cv::Point>& c, std::vector<cv::Point2f>& mc);

void drawBoundedRects(cv::Mat& src, int thresh)
{
 	cv::Mat threshold_output;
 	std::vector<std::vector<cv::Point> > contours;
 	std::vector<cv::Vec4i> hierarchy;
 
	// Convert src to gray format
	cv::cvtColor(src, threshold_output, CV_BGR2GRAY);

 	// Detect edges using Threshold
 	cv::threshold(threshold_output, threshold_output, thresh, 255, cv::THRESH_BINARY);
 	// Find contours
 	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
 
	// Get the moments
  	std::vector<cv::Moments> mu(contours.size() );
  	for( int i = 0; i < contours.size(); i++ )
     		{ mu[i] = moments( contours[i], false ); }

	//  Get the mass centers:
  	std::vector<cv::Point2f> mc( contours.size() );
  	for( int i = 0; i < contours.size(); i++ )
     		{ mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

 	// Approximate contours to rotated rectangles and ellipses
 	std::vector<cv::RotatedRect> minRect( contours.size());
 	for(int i = 0; i < contours.size(); i++)
 	{
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
 	}
 
 	// Draw polygonal contour + bounding rects
 	cv::Mat drawing = cv::Mat::zeros(threshold_output.size(), CV_8UC3);
 	for(int i = 0; i < contours.size(); i++)
	{
 		cv::Scalar color = cv::Scalar(0, 255, 255);
 		//cv::drawContours(drawing, contours, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
 		cv::Point2f rect_points[4]; 
		minRect[i].points(rect_points);
		for(int j = 0; j < 4; j++)
		{
			//cv::line(drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
		}
 	}

    for (int i = 0; i < mc.size(); i++)
    {
        //cv::circle(drawing, mc[i], 5, cv::Scalar(255, 255, 0));
    }


    double d = 0;
    std::vector<cv::Point> c;
	// Bounded rectangle is the one at the 0th index
	if (minRect.size() > 0)
		d = calculateDistance(drawing, minRect[0]);
    if (contours.size() > 0)
    {
        c = corners(contours[0], drawing);
        calcHorizAngle(drawing, d, c, mc);
    }

    /*
    for (int i = 0; i < c.size(); i++)
    {
        cv::circle(drawing, c[i], 5, cv::Scalar(0, 255, 0));
    }
    */

	// Calculate the area with the moments 00 and compare with the result of the OpenCV function
	// printf("\t Info: Area and Contour Length \n");
  	for (int i = 0; i < contours.size(); i++)
    {
        // printf(" * Contour[%2d] - Area (M_00) = %4.2f - Area OpenCV: %4.2f - Length: %4.2f\n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
        // printf("Contour[%2d] - Length: %4.2f\n", i, arcLength( contours[i], true ) );
		cv::Scalar color = cv::Scalar(0, 255, 0);
        if (i == 0)
        {
            cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }
	}

 	// Show in a window
 	cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
 	cv::imshow("Contours", drawing);
}

double distance(cv::Point one, cv::Point two)
{
    return std::sqrt(std::pow(one.x - two.x, 2) + std::pow(one.y - two.y, 2));
}

// Returns the two dimensional distance between the components of two points
double distance2D(double comp1, double comp2)
{
    return std::abs(comp2 - comp1);
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

    // Calibration phase
    //double hStraight = distance2D(c[0].y, c[1].y);
    // Note: dStraight may need to be calibrated too

    //double hLeft = distance2D(c[0].y, c[1].y);
    //double hRight = distance2D(c[2].y, c[3].y);
    //double hAvg = (hLeft + hRight) / 2;
    double wPixel = distance2D(c[0].x, c[3].x);
    double center = image.cols / 2;
    double distFromCenter = mc[0].x - center;
    double inchesPerPixel = 20 / wPixel;

    cv::circle(image, mc[0], 5, cv::Scalar(255, 255, 0));
	cv::line(image, cv::Point(center, mc[0].y), mc[0], cv::Scalar (255, 255, 100));


    //double yDist = distFromCenter + (wPixel / 2);
    double yDist = distFromCenter * inchesPerPixel;
    double theta = std::asin(yDist / xDist) * 180 / PI;

    //double alpha = std::atan(yTrans / dStraight) * 180 / PI;
    //double beta = std::atan((yTrans * (hStraight + hAvg)) / (dStraight * (hStraight - hRight))) * 180 / PI;

	char str[50];
	sprintf(str, "Horiz Angle  = %4.2f", theta);
    cv::putText(image, str, cv::Point(10, 380), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	//sprintf(str, "Beta    = %4.2f", beta);
    //cv::putText(image, str, cv::Point(10, 400), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);
}

double calculateDistance (cv::Mat& image, cv::RotatedRect& boundedRect)
{
	double focalLen = 675.0;
	// 20 inches real width
	double wReal = 20;
	double wPixel = 0;
	double d = 180;
	double theta = 0;
    const double TOWER_HEIGHT = 7.1;
    const double PI = 3.14159265;

	cv::Point2f vert[4];
	boundedRect.points(vert);
	cv::line(image, vert[0], vert[3], cv::Scalar (255, 0, 0));
	wPixel = distance(vert[0], vert[3]);
    // focalLen = (wPixel * d) / wReal;
	d = (wReal * focalLen) / wPixel;
    theta = asin(TOWER_HEIGHT / (d / 12)) * 180 / PI;

	char str[50];
	//sprintf(str, "Line Length  = %4.2f", wPixel);
    //cv::putText(image, str, cv::Point(10, 400), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Focal Length = %4.2f", focalLen);
    cv::putText(image, str, cv::Point(10, 420), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Distance     = %4.2f", d);
    cv::putText(image, str, cv::Point(10, 440), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

	sprintf(str, "Vert Angle   = %4.2f", theta);
    cv::putText(image, str, cv::Point(10, 460), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 0), 1, 8, false);

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

	// Parameters for applying filters even if windows are closed
	int apply_blur = 1;
	int apply_color = 1;
    int apply_dilate_erode = 0;
	int apply_edge = 1;
	int apply_laplacian = 0;
	int apply_hough = 0;
	int apply_depth_dist = 0;
	int apply_merge = 1;

	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 10;
	int sigmaY = 10;

	// hsvColorThreshold parameters
	int hMin = 120;
	int hMax = 200;
	int sMin = 0;
	int sMax = 40;
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

    // contour parameters
	int contours = 120;

	std::cout << "\n";
	std::cout << " =========== FILTER LIST =========== " << "\n";
	std::cout << "|                                   |" << "\n";
	std::cout << "| (0) No Filter                     |" << "\n";
	std::cout << "| (1) Gaussian Blur Filter          |" << "\n";
	std::cout << "| (2) HSV Color Filter              |" << "\n";
	std::cout << "| (3) Dilate and Erode Filter       |" << "\n";
	std::cout << "| (4) Canny Edge Detection Filter   |" << "\n";
	std::cout << "| (5) Laplacian Sharpen Filter      |" << "\n";
	std::cout << "| (6) Hough Lines Filter            |" << "\n";
	std::cout << "| (7) Merge Final Outputs           |" << "\n";
	std::cout << "|                                   |" << "\n";
	std::cout << " =================================== " << "\n";
	std::cout << "\n";

	std::cout << "\n";
	std::cout << " ============== NOTICE ============= " << "\n";
	std::cout << "|                                   |" << "\n";
	std::cout << "| Press 'q' to quit without saving  |" << "\n";
	std::cout << "| Press ' ' to pause                |" << "\n";
	std::cout << "|                                   |" << "\n";
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

    if (port == -1)
        return -1;

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
		selectMode(blur, color, dilate_erode, edge, laplacian, hough, depth_dist, merge);

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
		cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		cv::createTrackbar("Contours Threshold:", "Contours", &contours, 255);
		rgb.copyTo(image);

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
		drawBoundedRects(image, contours);
		cannyEdgeDetectWindows(image, threshLow, threshHigh, apply_edge, edge);
		laplacianSharpenWindows(image, ddepth, laplacian_ksize, scale, delta, apply_laplacian, laplacian);
		houghLinesWindows(image, rho, theta, threshold, lineMin, maxGap, apply_hough, hough);
		mergeFinalWindows(rgb, image, weight1, weight2, apply_merge, merge);
		kill = cv::waitKey(5);
	}
	return 0;	
}
