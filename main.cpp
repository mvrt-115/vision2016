#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "include/input/input.hpp"

#include "include/filters/RoboLaplacian.hpp"
#include "include/filters/RoboDilateErode.hpp"
#include "include/filters/RoboEdgeDetect.hpp"
#include "include/filters/RoboGaussianBlur.hpp"
#include "include/filters/RoboColorFilter.hpp"
#include "include/filters/RoboHoughLines.hpp"
#include "include/filters/select_mode.hpp"

int main()
{
	// cv::namedWindow("Blur Editor", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Color Filter Editor", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Color Filter Editor", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Dilate and Erode Editor", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Sharpen Editor", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Hough Lines Editor", cv::WINDOW_AUTOSIZE);	

	// // Editor to select which filter(s) to apply
	// int blur = 0;
	// int color = 0;
    // int dilate_erode = 0;
	// int edge = 0;
	// int laplacian = 0;
	// int hough = 0; 
	// cv::namedWindow("Efficiency Editor", cv::WINDOW_AUTOSIZE);
	// cv::createTrackbar("Blur Filter", "Efficiency Editor", &blur, 1);
	// cv::createTrackbar("Color Filter", "Efficiency Editor", &color, 1);
	// cv::createTrackbar("Dilate Erode Filter", "Efficiency Editor", &dilate_erode, 1);
	// cv::createTrackbar("Edge Filter", "Efficiency Editor", &edge, 1);
	// cv::createTrackbar("Sharpen Filter", "Efficiency Editor", &laplacian, 1);
	// cv::createTrackbar("Hough Line Filter", "Efficiency Editor", &hough, 1);

	// roboGaussianBlur parameters
	int blur_ksize = 1;
	int sigmaX = 0;
	int sigmaY = 0;
	cv::createTrackbar("Kernel Size", "Blur Editor", &blur_ksize, 10);
	cv::createTrackbar("Sigma X", "Blur Editor", &sigmaX, 100);
	cv::createTrackbar("Sigma Y", "Blur Editor", &sigmaY, 100);

	// roboColorFilter  parameters
	int hMin = 0;
	int hMax = 255;
	int sMin = 0;
	int sMax = 255;
	int vMin = 0;
	int vMax = 255;
	int debugMode = 0; //0 is none, 1 is debug, 2 is debug and debugpre	
	cv::createTrackbar("Hue Min", "Color Filter Editor", &hMin, 255);
	cv::createTrackbar("Hue Max", "Color Filter Editor", &hMax, 255);
	cv::createTrackbar("Sat Min", "Color Filter Editor", &sMin, 255);
	cv::createTrackbar("Sat Max", "Color Filter Editor", &sMax, 255);
	cv::createTrackbar("Val Min", "Color Filter Editor", &vMin, 255);
	cv::createTrackbar("Val Max", "Color Filter Editor", &vMax, 255);
	cv::createTrackbar("Debug Mode", "Color Filter Editor", &debugMode, 2);
	
	// roboErodeDilate parameters
	int holes = 1;
	int noise = 1;
	int size = 1;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
                      cv::Size(2 * size + 1, 2 * size + 1), 
                      cv::Point(size, size) );
	cv::createTrackbar("Noise Iterations", "Dilate and Erode Editor", &noise, 15);
	cv::createTrackbar("Filter Size", "Dilate and Erode Editor", &size, 10);
	cv::createTrackbar("Hole Iterations", "Dilate and Erode Editor", &holes, 15);
	
	// roboEdgeDetect parameters
	int threshLow = 0;
	int threshHigh = 255;
	int edge_ksize = 1;
	cv::createTrackbar("Kernel Size", "Edge Editor", &edge_ksize, 7);
	cv::createTrackbar("Bottom Threshold", "Edge Editor", &threshLow, 255);
	cv::createTrackbar("Upper Threshold", "Edge Editor", &threshHigh, 255);
	
	// roboLaplacian parameters
	int laplacian_ksize = 1;
	int scale = 0; // optional scale value added to image
	int delta = 0; // optional delta value added to image
	int ddepth = CV_16S;
	cv::createTrackbar("Kernel Size", "Sharpen Editor", &laplacian_ksize, 9);
	cv::createTrackbar("Scale", "Sharpen Editor", &scale, 9);
	cv::createTrackbar("Delta", "Sharpen Editor", &delta, 9);
	
	// roboHoughLines parameters
	int rho = 1;
	int theta = 180;
	int threshold = 49;
	int lineMin = 9;
	int maxGap = 49;
	cv::createTrackbar("Theta", "Hough Lines Editor", &theta, 180);
	cv::createTrackbar("Rho", "Hough Lines Editor", &rho, 100);
	cv::createTrackbar("Threshold", "Hough Lines Editor", &threshold, 199);
	cv::createTrackbar("LineMin", "Hough Lines Editor", &lineMin, 399);
	cv::createTrackbar("MaxGap", "Hough Lines Editor", &maxGap, 399);	

	Input input; // Class holding input methods

	cv::Mat rgb, rgb_orig, depth, ir;
	// Note, IR does not seem to work with fakenect

	// cv::namedWindow("BGR Feed", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Depth Feed", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("IR Feed", cv::WINDOW_AUTOSIZE);

	// decide weighted sums of original rgb feed to filtered rgb feed
	int weight1 = 1;
	int weight2 = 1;

	for (;;)
	{
		// Reset all feed output windows
		cv::destroyWindow("All Filtered");
		
		// Input rgb, depth, and ir feeds
		input.getBGR(rgb);
		rgb_orig = rgb.clone(); // Clone rgb input in order to merge at end
		// cv::imshow("BGR Feed", rgb);
		
		input.getDepth(depth);
		// cv::imshow("Depth Feed", depth);
	
		// input.getIR(ir);
		// cv::imshow("IR Feed", ir);
		
		// Mat * channels = new Mat[3];
		
		if (blur) {
			roboGaussianBlur(rgb, blur_ksize, sigmaX, sigmaY);
			cv::imshow("Gaussian Blur Output", rgb);
		} else if (!blur) {
		 	cv::destroyWindow("Gaussian Blur Output");
		}

		if (color) {
			roboColorFilter(rgb, hMin, hMax, sMin, sMax, vMin, vMax);
			cv::imshow("Color Filter Output", rgb);
		} else if (!color) {
			 cv::destroyWindow("Color Filter Output");
		} 
		
		if (dilate_erode) {
			roboDilateErode(rgb, element, holes, noise);
			cv::imshow("Dilate and Erode Output", rgb);
		} else if (!dilate_erode) {
			 cv::destroyWindow("Dilate and Erode Output");
		}

		if (edge) {
			roboEdgeDetect(rgb, edge_ksize, threshLow, threshHigh);
			cv::imshow("Edge Detection Output", rgb);
		} else if (!edge) {
			cv::destroyWindow("Edge Detection Output");
		}

		if (laplacian) {
			roboLaplacian(rgb, ddepth, laplacian_ksize, scale, delta);
			cv::imshow("Laplacian Output", rgb);
		} else if (!laplacian) {
		 	cv::destroyWindow("Laplacian Output");
		}

		if (hough) { 
			roboHoughLines(rgb, rho, theta, threshold, lineMin, maxGap);
			cv::imshow("Hough Lines Output (All Filtered)", rgb);		
		} else if (!hough) { 		
			cv::destroyWindow("Hough Lines Output (All Filtered)");
		} else {		
			cv::imshow("All Filtered", rgb);
		}

		addWeighted(rgb, weight1, rgb_orig, weight2, 3, rgb);
		//cv::imshow("Final Weighted Average", rgb);
		//cv::imshow("Original", rgb_orig);
		//delete [] channels;
		
		cv::waitKey(1);
	}
	std::cout << "test";

	return 0;	
}
