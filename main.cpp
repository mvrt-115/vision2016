#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
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

int main( int argc, char *argv[] )
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
	int sMax = 100;
	int vMin = 0;
	int vMax = 100;
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
	while ((kill != 'q') && (kill != 's'))
	{
		if (kill == ' ') // Press space to pause program, then any key to resume
		{
			cv::waitKey(0);
		}
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

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
		cannyEdgeDetectWindows(image, threshLow, threshHigh, apply_edge, edge);
		laplacianSharpenWindows(image, ddepth, laplacian_ksize, scale, delta, apply_laplacian, laplacian);
#if 0
		// List sizes of image
		cv::Size imageSize = image.size();
		std::cout << "Image: [" << imageSize.height << " x " << imageSize.width << "]" << "\n";
#endif
		houghLinesWindows(image, rho, theta, threshold, lineMin, maxGap, apply_hough, hough);
#if 0
		// List sizes of image
		imageSize = image.size();
		std::cout << "Image: [" << imageSize.height << " x " << imageSize.width << "]" << "\n";
#endif
        mergeFinalWindows(rgb_orig, image, weight1, weight2, apply_merge, merge);
		kill = cv::waitKey(5);
	}
	return 0;	
}
