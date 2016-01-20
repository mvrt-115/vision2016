#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>

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

void drawBoundedRects(cv::Mat& src, int thresh)
{
 	cv::Mat threshold_output;
 	std::vector<std::vector<cv::Point> > contours;
 	std::vector<cv::Vec4i> hierarchy;
 
	// Convert src to gray format
	cv::cvtColor(src, threshold_output, CV_BGR2GRAY);

 	/// Detect edges using Threshold
 	cv::threshold(threshold_output, threshold_output, thresh, 255, cv::THRESH_BINARY);
 	/// Find contours
 	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
 
	/// Get the moments
  	cv::vector<cv::Moments> mu(contours.size() );
  	for( int i = 0; i < contours.size(); i++ )
     		{ mu[i] = moments( contours[i], false ); }

	///  Get the mass centers:
  	cv::vector<cv::Point2f> mc( contours.size() );
  	for( int i = 0; i < contours.size(); i++ )
     		{ mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

 	/// Approximate contours to rotated rectangles and ellipses
 	std::vector<cv::RotatedRect> minRect( contours.size());
 	for(int i = 0; i < contours.size(); i++)
 	{
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
 	}
 
 	// Draw polygonal contour + bounding rects
 	cv::Mat drawing = cv::Mat::zeros(threshold_output.size(), CV_8UC3);
 	for(int i = 0; i < contours.size(); i++)
 	{
 		cv::Scalar color = cv::Scalar(0, 0, 255);
 		cv::drawContours(drawing, contours, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
 		cv::Point2f rect_points[4]; minRect[i].points(rect_points);
		for(int j = 0; j < 4; j++)
		{
			cv::line(drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
		}
 	}

	// Calculate the area with the moments 00 and compare with the result of the OpenCV function
	printf("\t Info: Area and Contour Length \n");
  	for( int i = 0; i< contours.size(); i++ )
     	{
       		printf(" * Contour[%2d] - Area (M_00) = %4.2f - Area OpenCV: %4.2f - Length: %4.2f\n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
		cv::Scalar color = cv::Scalar(0, 255, 0);
 		cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
	}

 	/// Show in a window
 	cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
 	cv::imshow("Contours", drawing);
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
	int apply_blur = 0; 
	int apply_color = 0;
    	int apply_dilate_erode = 0;
	int apply_edge = 0;
	int apply_laplacian = 0;
	int apply_hough = 0; 
	int apply_depth_dist = 0;
	int apply_merge = 0; 

	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 10;
	int sigmaY = 10;

	// hsvColorThreshold parameters
	int hMin = 0;
	int hMax = 360;
	int sMin = 0;
	int sMax = 255;
	int vMin = 0;
	int vMax = 255;
	int debugMode = 0; // 0 is none, 1 is debug mode
	int bitAnd = 1; // 0 is none, 1 is bitAnd between h, s, and v

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
	int scale = 1; // optional scale value added to image
	int delta = 0; // optional delta value added to image
	int ddepth = CV_16S;
	
	// houghLines parameters
	int rho = 1;
	int theta = 180;
	int threshold = 50;
	int lineMin = 50;
	int maxGap = 10;

	// mergeFinal parameters
	int weight1 = 100; // decide weighted sums of original rgb feed to filtered rgb feed; values = out of 100
	int weight2 = 100;
	int merge_type = 0; // 0 is none, 1 is rgb + rgb_orig, 2 is depth + depth_orig, 3 is rgb + depth 

	// depthDistance parameters
	int center_x = 320; // point to calculate distance of
	int center_y = 240;
	int orig_x = 20; // point to display depth information text
	int orig_y = 450;

	std::cout << std::endl;
	std::cout << " =========== FILTER LIST =========== " << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << "| (0) No Filter                     |" << std::endl;
	std::cout << "| (1) Gaussian Blur Filter          |" << std::endl;
	std::cout << "| (2) HSV Color Filter              |" << std::endl;
	std::cout << "| (3) Dilate and Erode Filter       |" << std::endl;
	std::cout << "| (4) Canny Edge Detection Filter   |" << std::endl;
	std::cout << "| (5) Laplacian Sharpen Filter      |" << std::endl;
	std::cout << "| (6) Hough Lines Filter            |" << std::endl;
	std::cout << "| (7) Merge Final Outputs           |" << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << " =================================== " << std::endl;
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << " ============== NOTICE ============= " << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << "| Press 'q' to quit without saving  |" << std::endl;
	std::cout << "| Press ' ' to pause                |" << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << " =================================== " << std::endl;
	std::cout << std::endl;

	cv::VideoCapture camera(0);
	if( !camera.isOpened() && (argc < 2) )
	{
		std::cout << "Error - Unable to open Camera at Port 0" << std::endl;
		return -1;
	}

	// Matrices for holding image data
	cv::Mat rgb, rgb_orig;
	cv::Mat image;

	char kill = ' '; // Press q to quit the program
	
	int thresh = 0;

	while ((kill != 'q') && (kill != 's'))
	{
		if (kill == ' ') // Press space to pause program, then any key to resume
		{
			cv::waitKey(0);
		}

		// Create all the calibration windows
		selectMode(blur, color, dilate_erode, edge, laplacian, hough, depth_dist, merge);
		
		if (argc > 2) // Use images
		{
			rgb = cv::imread(argv[1]);
			rgb_orig = rgb.clone(); // Clone rgb input in order to merge at end

			if (!rgb.data || !rgb_orig.data) // No data
			{
				std::cout << "No image data" << std::endl;
				return -1;
			}
		}
		else 
		{
			camera >> rgb;
			rgb_orig = rgb.clone();
		}
		
		cv::imshow("BGR Feed", rgb);
		rgb.copyTo(image);

		cv::namedWindow("Source", CV_WINDOW_AUTOSIZE);
		cv::createTrackbar("Threshold:", "Source", &thresh, 255);

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
		drawBoundedRects(image, thresh);
		cannyEdgeDetectWindows(image, threshLow, threshHigh, apply_edge, edge);
		laplacianSharpenWindows(image, ddepth, laplacian_ksize, scale, delta, apply_laplacian, laplacian);
	
#if 0
		// List sizes of images
		cv::Size rgbSize = rgb.size();
		std::cout << "RGB:   [" << rgbSize.height << " x " << rgbSize.width <<  "]" << std::endl;
		cv::Size depthSize = depth.size();
		std::cout << "Depth: [" << depthSize.height << " x " << depthSize.width << "]" << std::endl;
		cv::Size imageSize = image.size();
		std::cout << "Image: [" << imageSize.height << " x " << imageSize.width << "]" << std::endl;
#endif
		houghLinesWindows(image, rho, theta, threshold, lineMin, maxGap, apply_hough, hough);
	
#if 0
		// List sizes of images
		rgbSize = rgb.size();
		std::cout << "RGB:   [" << rgbSize.height << " x " << rgbSize.width <<  "]" << std::endl;
		depthSize = depth.size();
		std::cout << "Depth: [" << depthSize.height << " x " << depthSize.width << "]" << std::endl;
		imageSize = image.size();
		std::cout << "Image: [" << imageSize.height << " x " << imageSize.width << "]" << std::endl;
#endif

		depthDistanceWindows(image, center_x, center_y, orig_x, orig_y, apply_depth_dist, depth_dist);

		kill = cv::waitKey(5);
	}
	return 0;	
}
