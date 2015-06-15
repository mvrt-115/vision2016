#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <queue>
#include <iostream>
#include <string>

#include "include/logs/writeToFile.hpp"
#include "include/logs/currentDateTime.hpp"

#include "include/input/input.hpp"

#include "include/filters/selectMode.hpp"
#include "include/filters/roboGaussianBlurWindows.hpp"
#include "include/filters/roboColorFilterWindows.hpp"
#include "include/filters/roboDilateErodeWindows.hpp"
#include "include/filters/roboEdgeDetectWindows.hpp"
#include "include/filters/roboLaplacianWindows.hpp"
#include "include/filters/roboHoughLinesWindows.hpp"

int main()
{
	// Parameters for selecting which filters to apply
	int blur = 0;
	int color = 0;
    int dilate_erode = 0;
	int edge = 0;
	int laplacian = 0;
	int hough = 0; 

	// roboGaussianBlur parameters
	int blur_ksize = 1;
	int sigmaX = 0;
	int sigmaY = 0;

	// roboColorFilter parameters
	int hMin = 0;
	int hMax = 255;
	int sMin = 0;
	int sMax = 255;
	int vMin = 0;
	int vMax = 255;
	int debugMode = 0; //0 is none, 1 is debug, 2 is debug and debugpre	
	
	// roboDilateErode parameters
	int holes = 1;
	int noise = 1;
	int size = 1;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
                      cv::Size(2 * size + 1, 2 * size + 1), 
                      cv::Point(size, size) );
	
	// roboEdgeDetect parameters
	int threshLow = 0;
	int threshHigh = 255;
	int edge_ksize = 1;
	
	// roboLaplacian parameters
	int laplacian_ksize = 3;
	int scale = 0; // optional scale value added to image
	int delta = 0; // optional delta value added to image
	int ddepth = CV_16S;
	
	// roboHoughLines parameters
	int rho = 1;
	int theta = 180;
	int threshold = 49;
	int lineMin = 9;
	int maxGap = 49;

	Input input; // Class holding input methods

	cv::Mat rgb, rgb_orig, depth, ir;
	// Note, IR does not seem to work with fakenect

	// cv::namedWindow("BGR Feed", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("Depth Feed", cv::WINDOW_AUTOSIZE);
	// cv::namedWindow("IR Feed", cv::WINDOW_AUTOSIZE);

	// decide weighted sums of original rgb feed to filtered rgb feed
	int weight1 = 1;
	int weight2 = 1;

	char kill = ' '; // Press q to quit the program
	std::cout << "Press q to quit" << std::endl;

	while (kill != 'q')
	{
		// Create all the calibration windows
		selectMode(blur, color, dilate_erode, edge, laplacian, hough);

		// Reset all the feed output windows
		// cv::destroyWindow("All Filtered");
		
		// Input rgb, depth, and ir feeds
		input.getBGR(rgb);
		rgb_orig = rgb.clone(); // Clone rgb input in order to merge at end
		cv::imshow("BGR Feed", rgb);
		
		input.getDepth(depth);
		cv::imshow("Depth Feed", depth);
	
		roboGaussianBlurWindows(rgb, blur_ksize, sigmaX, sigmaY, blur);
		roboColorFilterWindows(rgb, hMin, hMax, sMin, sMax, vMin, vMax, color);
		roboDilateErodeWindows(rgb, element, holes, noise, dilate_erode);
		roboEdgeDetectWindows(rgb, edge_ksize, threshLow, threshHigh, edge);
		roboLaplacianWindows(rgb, ddepth, laplacian_ksize, scale, delta, laplacian);
		roboHoughLinesWindow(rgb, rho, theta, threshold, lineMin, maxGap, hough);

		//--- IR DOESN'T WORK WITH FAKENECT ---//
		// input.getIR(ir);
		// cv::imshow("IR Feed", ir);
		
		// addWeighted(rgb, weight1, rgb_orig, weight2, 3, rgb);
		// cv::imshow("Final Weighted Average", rgb);
		
		kill = cv::waitKey(1);
	}
	
	std::queue <int> filter_params; // construct a queue for writing out all the parameter values to a file (run.log)
	
	// roboGaussianBlur parameters
	filter_params.push(blur_ksize);
	filter_params.push(sigmaX);
	filter_params.push(sigmaY);

	// roboColorFilter parameters
	filter_params.push(hMin);
	filter_params.push(hMax);
	filter_params.push(sMin);
	filter_params.push(sMax);
	filter_params.push(vMin);
	filter_params.push(vMax);
	//filter_params.push(debugMode);
	
	// roboDilateErode parameters
	filter_params.push(holes);
	filter_params.push(noise);
	filter_params.push(size);

	// roboEdgeDetect parameters
	filter_params.push(threshLow);
	filter_params.push(threshHigh);
	//filter_params.push(edge_ksize);
	
	// roboLaplacian parameters
	filter_params.push(laplacian_ksize);
	filter_params.push(scale);
	filter_params.push(delta);
	filter_params.push(ddepth);

	// roboHoughLines parameters
	filter_params.push(rho);
	filter_params.push(theta);
	filter_params.push(threshold);
	filter_params.push(lineMin);
	filter_params.push(maxGap);

	std::string answer;
	std::string filename;

	std::cout << std::endl <<  "Would you like to name the file? (Y/N): ";
	std::cin >> answer;

	if ((answer == "y") || (answer == "Y") || (answer == "Yes") || (answer == "YES"))
	{
		std::cout << "What would you like to name the file?: ";
		std::cin >> filename;
	}
	else 
	{
		filename = currentDateTime();
	}
	writeToFile ("logs/", filename, filter_params); // file path, file name, queue
	std::cout << "File successfully made at 'logs/" << filename << "'" << std::endl << std::endl;	
	return 0;	
}
