#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <queue>
#include <iostream>
#include <string>

#include "include/logging/writeToFile.hpp"
#include "include/logging/readFromFile.hpp"
#include "include/logging/currentDateTime.hpp"
#include "include/logging/listFiles.hpp"
#include "include/logging/enumCvType.hpp"

#include "include/input/input.hpp"

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
	int choose_img = 0; // 0 is RGB, 1 is Depth; Select image to filter
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
	int hMax = 255;
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

	int start = 0; // Option for selecting type of data
	std::queue <int> filter_params; // queue for storing all the parameter values for writing and reading

	namespace fs = boost::filesystem; // find full path of the folder this file is in
	fs::path full_path ( fs::initial_path<fs::path>() );	
	full_path = fs::system_complete ( fs::path("logs/") ); // append logs/ 
		
	std::cout << std::endl;
	std::cout << " ============ START MENU =========== " << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << "| 0. Default Data                   |" << std::endl;
	std::cout << "| 1. Stored Data                    |" << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << " =================================== " << std::endl;
	std::cout << std::endl;
	
	std::cout << " Select Mode: ";
	std::cin >> start;

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
	std::cout << "| Press 's' to quit with saving     |" << std::endl;
	std::cout << "|                                   |" << std::endl;
	std::cout << " =================================== " << std::endl;
	std::cout << std::endl;

	if (start)
   	{
		// Arrays for holding paths to directories, files, and others from listFiles()
		std::string *dir_array = new std::string [30];
		std::string	*file_array = new std::string [30];
		std::string *other_array = new std::string [30]; 
	
		listFiles("logs/", dir_array, file_array, other_array);

		std::cout << "Number of existing data to use (e.g. '1'): ";
		
		int pre_data_num; // User selects data to use
		std::cin >> pre_data_num;

		std::string pre_data = file_array[pre_data_num];
		readFromFile(pre_data, filter_params);

		// Filter options parameters
		choose_img = filter_params.front();
			filter_params.pop();
		blur = filter_params.front();
			filter_params.pop();
		color = filter_params.front();
			filter_params.pop();
		dilate_erode = filter_params.front();
			filter_params.pop();
		edge = filter_params.front();
			filter_params.pop();
		laplacian = filter_params.front();
			filter_params.pop();
		hough = filter_params.front();
			filter_params.pop();
		depth_dist = filter_params.front();
			filter_params.pop();
		merge = filter_params.front();
			filter_params.pop();

		// Applying filters parameters
		apply_blur = filter_params.front();
			filter_params.pop();
		apply_color = filter_params.front();
			filter_params.pop();
		apply_dilate_erode = filter_params.front();
			filter_params.pop();
		apply_edge = filter_params.front();
			filter_params.pop();
		apply_laplacian = filter_params.front();
			filter_params.pop();
		apply_hough = filter_params.front();
			filter_params.pop();
		apply_depth_dist = filter_params.front();
			filter_params.pop();
		apply_merge = filter_params.front();
			filter_params.pop();

		// gaussianBlur parameters
		blur_ksize = filter_params.front();
			filter_params.pop();
		sigmaX = filter_params.front();
			filter_params.pop();
		sigmaY = filter_params.front();
			filter_params.pop();

		// hsvColorThreshold parameters
		hMin = filter_params.front();
			filter_params.pop();
		hMax = filter_params.front();
			filter_params.pop();
		sMin = filter_params.front();
			filter_params.pop();
		sMax = filter_params.front();
			filter_params.pop();
		vMin = filter_params.front();
			filter_params.pop();
		vMax = filter_params.front();
			filter_params.pop();
		debugMode = filter_params.front();
			filter_params.pop();
		
		// dilateErode parameters
		holes = filter_params.front();
			filter_params.pop();
		noise = filter_params.front();
			filter_params.pop();
		size = filter_params.front();
			filter_params.pop();

		// cannyEdgeDetect parameters
		threshLow = filter_params.front();
			filter_params.pop();
		threshHigh = filter_params.front();
			filter_params.pop();
		
		// laplacianSharpen parameters
		laplacian_ksize = filter_params.front();
			filter_params.pop();
		scale = filter_params.front();
			filter_params.pop();
		delta = filter_params.front();
			filter_params.pop();
		ddepth = filter_params.front();
			filter_params.pop();

		// houghLines parameters
		rho = filter_params.front();
			filter_params.pop();
		theta = filter_params.front();
			filter_params.pop();
		threshold = filter_params.front();
			filter_params.pop();
		lineMin = filter_params.front();
			filter_params.pop();
		maxGap = filter_params.front();
			filter_params.pop();

		// mergeFinal parameters
		weight1 = filter_params.front();
			filter_params.pop();
		weight2 = filter_params.front();
			filter_params.pop();
		merge_type = filter_params.front();
			filter_params.pop();

		// depthDistance parameters
		center_x = filter_params.front();
			filter_params.pop();
		center_y = filter_params.front();
			filter_params.pop();
		orig_x = filter_params.front();
			filter_params.pop();
		orig_y = filter_params.front();
			filter_params.pop();	
	}

	Input input; // Class holding input methods for fakenect

	cv::VideoCapture camera(0);
	if( !camera.isOpened() && (argc < 2) )
	{
		std::cout << "Error - Unable to open Camera at Port 0" << std::endl;
		return -1;
	}

	// Matrices for holding image data
	cv::Mat rgb, rgb_orig;
	cv::Mat	depth, depth_orig;
	cv::Mat image;
	// cv::Mat ir; 

	char kill = ' '; // Press q to quit the program

	while ((kill != 'q') && (kill != 's'))
	{
		if (kill == ' ') // Press space to pause program, then any key to resume
		{
			cv::waitKey(0);
		}

		// Create all the calibration windows
		selectMode(choose_img, blur, color, dilate_erode, edge, laplacian, hough, depth_dist, merge);
		
		if (argc > 2) // Use images
		{
			rgb = cv::imread(argv[1]);
			rgb_orig = rgb.clone(); // Clone rgb input in order to merge at end

			depth = cv::imread(argv[2]);
			depth_orig = depth.clone(); // Clone depth input in order to merge at end
			
			if ( !rgb.data || !rgb_orig.data || !depth.data || !depth_orig.data ) // No data
			{
                std::cout << "No image data" << std::endl;
				return -1;
			}
		}

		else if (argc > 1) // Use camera; argc is a dummy argument in order to differentiate between fakenect and videocapture
		{
			camera >> rgb;
			rgb_orig = rgb.clone();
			
			depth = cv::Mat::zeros(480, 640, CV_8U); // Image is all black
			depth_orig = depth.clone();	
		}
		
		else // Use fakenect
		{
			// Input rgb, depth, and ir feeds
			input.getBGR(rgb);
			rgb_orig = rgb.clone(); // Clone rgb input in order to merge at end
			
			input.getDepth(depth);
			depth_orig = depth.clone(); // Clone depth input in order to merge at end
		
			//--- IR DOESN'T WORK WITH FAKENECT (need confirm) ---//
			// input.getIR(ir);
		}
		
		cv::imshow("BGR Feed", rgb);
		cv::imshow("Depth Feed", depth);
		// cv::imshow("IR Feed", ir);

		// Choose which image to filter
		if 		(choose_img == 0)    rgb.copyTo(image);
		else if (choose_img == 1)	 depth.copyTo(image);

		// Filters are only applied if last parameter is true, otherwise their windows are destroyed
		gaussianBlurWindows(image, blur_ksize, sigmaX, sigmaY, apply_blur, blur);
		hsvColorThresholdWindows(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd, apply_color, color);
		dilateErodeWindows(image, element, holes, noise, apply_dilate_erode, dilate_erode);
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

		if 		(choose_img == 0)	mergeFinalWindows(image, rgb_orig, depth, depth_orig, image, weight1, weight2, merge_type, apply_merge, merge);
		else if (choose_img == 1)	mergeFinalWindows(rgb, rgb_orig, image, depth_orig, image, weight1, weight2, merge_type, apply_merge, merge);
		
		depthDistanceWindows(image, center_x, center_y, orig_x, orig_y, apply_depth_dist, depth_dist);

		kill = cv::waitKey(5);
	}

    // Filter options parameters
	filter_params.push(choose_img);
	filter_params.push(blur);
	filter_params.push(color);
	filter_params.push(dilate_erode);
	filter_params.push(edge);
	filter_params.push(laplacian);
	filter_params.push(hough);
	filter_params.push(merge);
	
	// Applying filters parameters
	filter_params.push(apply_blur);
	filter_params.push(apply_color);
	filter_params.push(apply_dilate_erode);
	filter_params.push(apply_edge);
	filter_params.push(apply_laplacian);
	filter_params.push(apply_hough);
	filter_params.push(apply_merge);
	
	// gaussianBlur parameters
	filter_params.push(blur_ksize);
	filter_params.push(sigmaX);
	filter_params.push(sigmaY);

	// hsvColorThreshold parameters
	filter_params.push(hMin);
	filter_params.push(hMax);
	filter_params.push(sMin);
	filter_params.push(sMax);
	filter_params.push(vMin);
	filter_params.push(vMax);
	filter_params.push(debugMode);
	
	// dilateErode parameters
	filter_params.push(holes);
	filter_params.push(noise);
	filter_params.push(size);

	// cannyEdgeDetect parameters
	filter_params.push(threshLow);
	filter_params.push(threshHigh);
	
	// laplacianSharpen parameters
	filter_params.push(laplacian_ksize);
	filter_params.push(scale);
	filter_params.push(delta);
	filter_params.push(ddepth);

	// houghLines parameters
	filter_params.push(rho);
	filter_params.push(theta);
	filter_params.push(threshold);
	filter_params.push(lineMin);
	filter_params.push(maxGap);

	// mergeFinal parameters
	filter_params.push(weight1);
	filter_params.push(weight2);
	filter_params.push(merge_type);

	// depthDistance parameters
	filter_params.push(center_x);
	filter_params.push(center_y);
	filter_params.push(orig_x);
	filter_params.push(orig_y);

	std::string answer;
	std::string filename;
	
	if (kill == 's') 
	{
		std::cout << std::endl <<  "Would you like to name the file? (Y/N): ";
		std::cin >> answer;

		if ((answer == "y") || (answer == "Y") || (answer == "yes") || (answer == "Yes") || (answer == "YES"))
		{
			std::cout << "What would you like to name the file? (e.g. 'testing'): ";
			std::cin >> filename;
		}
		else 
		{
			filename = currentDateTime();
		}
		writeToFile (full_path.string() + filename, filter_params); // file path, file name, queue
		std::cout << std::endl << "Successfully made file: '" << full_path.string() << filename << "'" << std::endl;	

		std::cout << std::endl << "Would you like to save the filtered rgb and depth images? (Y/N): ";
		std::cin >> answer;

		if ((answer == "y") || (answer == "Y") || (answer == "yes") || (answer == "Yes") || (answer == "YES"))
		{
			std::cout << "What would you like to name the images? (e.g. 'rgb-test depth-test': ";

			std::string image1, image2;
			
			std::cin >> image1;
			std::cin >> image2;

		    // Add file extensions and directory
			image1 = "images/" + image1 + ".png";
			image2 = "images/" + image2 + ".png";
			
			cv::imwrite(image1, rgb);
			cv::imwrite(image2, depth);
			
			std::cout << image1 << " " << image2 << std::endl;
		}
	}

	return 0;	
}
