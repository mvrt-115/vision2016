#include "filters/mergeFinal.hpp"
#include "logging/enumCvType.hpp"

void mergeFinal(cv::Mat &color, cv::Mat &color_orig, cv::Mat &depth, cv::Mat &depth_orig, cv::Mat &final_img, int &weight1, int &weight2, int &merge_type)
{
	// std::cout << std::endl;
	// std::cout << "============== FIRST ==============" << std::endl;
	// std::cout << "Color Pre-Filters Type:    " << enumCvType(color_orig) << std::endl;
	// std::cout << "Color Post-Filters Type:   " << enumCvType(color) 	 << std::endl;
	// std::cout << "Depth Pre-Filters Type:    " << enumCvType(depth_orig) << std::endl;
	// std::cout << "Depth Post-Filters Type:   " << enumCvType(depth) 	 << std::endl;
	// std::cout << std::endl;

	if (enumCvType(color_orig) == "CV_8UC1") 
	{
		cv::Mat *channels = new cv::Mat [3];
	
		// Make color_orig into CV_8UC3
		channels[0] = color_orig;
		channels[1] = channels[0];
		channels[2] = channels[0];

		cv::merge(channels, 3, color_orig);
			
		delete [] channels;
	}
	
	if (enumCvType(color) == "CV_8UC1") 
	{
		cv::Mat *channels = new cv::Mat [3];
	
		// Make color into CV_8UC3
		channels[0] = color;
		channels[1] = channels[0];
		channels[2] = channels[0];

		cv::merge(channels, 3, color);
			
		delete [] channels;
	}
	
	if (enumCvType(depth_orig) == "CV_8UC1")
	{
		cv::Mat *channels = new cv::Mat [3];
	
		// Make depth_orig into CV_8UC3
		channels[0] = depth_orig;
		channels[1] = channels[0];
		channels[2] = channels[0];

		cv::merge(channels, 3, depth_orig);
			
		delete [] channels;
	}
	
	if (enumCvType(depth) == "CV_8UC1")
	{
		cv::Mat *channels = new cv::Mat [3];
	
		// Make depth into CV_8UC3
		channels[0] = depth;
		channels[1] = channels[0];
		channels[2] = channels[0];

		cv::merge(channels, 3, depth);
			
		delete [] channels;
	}

	//std::cout << std::endl;
	//std::cout << "============= SECOND =============" << std::endl;
	//std::cout << "Color Pre-Filters Type:    " << enumCvType(color_orig) << std::endl;
	//std::cout << "Color Post-Filters Type:   " << enumCvType(color) 	 << std::endl;
	//std::cout << "Depth Pre-Filters Type:    " << enumCvType(depth_orig) << std::endl;
	//std::cout << "Depth Post-Filters Type:   " << enumCvType(depth) 	 << std::endl;
	//std::cout << std::endl;

	if (merge_type == 1) // Merge color images with pre-filtered color images
	{
		addWeighted(color, (double) weight1 / 100, color_orig, (double) weight2 / 100, 3, final_img);
	}

	else if (merge_type == 2) // Merge depth images with pre-filtered depth images
	{
		addWeighted(depth, (double) weight1 / 100, depth_orig, (double) weight2 / 100, 3, final_img);
	}

	else if (merge_type == 3) // Merge final depth images with final color images
	{
		addWeighted(color, (double) weight1 / 100, color_orig, (double) weight2 / 100, 3, color); // color + color_orig
		addWeighted(depth, (double) weight1 / 100, depth_orig, (double) weight2 / 100, 3, depth); // depth + depth_orig
		addWeighted(color, (double) weight1 / 100, depth, (double) weight2 / 100, 3, final_img); // color + depth
	}
}
