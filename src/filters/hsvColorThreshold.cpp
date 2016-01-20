#include "filters/hsvColorThreshold.hpp"

void hsvColorThreshold(cv::Mat &in, int hMin, int hMax, int sMin, int sMax, int vMin, int vMax, bool DEBUG, bool bitAnd)
{
	bool hueAltered = false;
	bool satAltered = false;
	bool valAltered = false;

	cvtColor(in, in, CV_BGR2HSV);
	cv::Mat *channels = new cv::Mat [3];
	cv::split(in, channels);

    // Hue is stored as 0 to 180, but is shown to user as 0 to 360
	if (hMin != 0 || hMax != 360)
	{
		hueAltered = true;
        hMin = static_cast<int>(static_cast<double>(hMin) / 2);
        hMax = static_cast<int>(static_cast<double>(hMax) / 2);
		cv::inRange(channels[0], cv::Scalar(hMin), cv::Scalar(hMax), channels[0]);
		
		if (DEBUG)
	   	{
			cv::namedWindow("Hue-Filtered Debug", cv::WINDOW_AUTOSIZE);
			cv::imshow("Hue-Filtered Debug", channels[0]);
		}	
		else if (!DEBUG)
		{
			cv::destroyWindow("Hue-Filtered Debug");
		}
	}
	else if ( (!DEBUG) || (hMin == 0 && hMax == 360) )
	{
		cv::destroyWindow("Hue-Filtered Debug");
	}

    // Saturation is stored as 0 to 255, but is shown to user as 0 to 100
	if (sMin != 0 || sMax != 100)
	{
		satAltered = true;
        sMin = static_cast<int>(static_cast<double>(sMin) * 255 / 100);
        sMax = static_cast<int>(static_cast<double>(sMax) * 255 / 100);
		cv::inRange(channels[1], cv::Scalar(sMin), cv::Scalar(sMax), channels[1]);
		
		if (DEBUG)
	   	{
		   	cv::imshow("Saturation-Filtered Debug", channels[1]);
		}
		else if (!DEBUG)
		{
			cv::destroyWindow("Saturation-Filtered Debug");
		}
	}
	else if ( (!DEBUG) || (sMin == 0 && sMax == 100) )
	{
		cv::destroyWindow("Saturation-Filtered Debug");
	}

    // Value is stored as 0 to 255, but is shown to user as 0 to 100
	if (vMin != 0 || vMax != 100)
	{
		valAltered = true;
        vMin = static_cast<int>(static_cast<double>(vMin) * 255 / 100);
        vMax = static_cast<int>(static_cast<double>(vMax) * 255 / 100);
		cv::inRange(channels[2], cv::Scalar(vMin), cv::Scalar(vMax), channels[2]);
		
		if (DEBUG)
	   	{ 
			cv::namedWindow("Value-Filtered Debug", cv::WINDOW_AUTOSIZE);
			cv::imshow("Value-Filtered Debug", channels[2]);
		}
		else if (!DEBUG)
		{
			cv::destroyWindow("Value-Filtered Debug");	
		}
	}
	else if ( (!DEBUG) || (vMin == 0 && vMax == 100) )
	{
		cv::destroyWindow("Value-Filtered Debug");	
	}

	if (bitAnd)
	{
		if (hueAltered && valAltered && satAltered) // Sat, Value, Hue
		{
			bitwise_and(channels[0], channels[1], channels[0]);
			bitwise_and(channels[0], channels[2], channels[0]);
			channels[1] = channels[0].clone();
			channels[2] = channels[0].clone();
		}

		else if (valAltered && hueAltered) // Value, Hue
		{
			bitwise_and(channels[0], channels[2], channels[0]);
			channels[2] = channels[0].clone();
			channels[1] = channels[0].clone();
		}

		else if (hueAltered && satAltered) // Sat, Hue
		{
			bitwise_and(channels[0], channels[1], channels[0]);
			channels[2] = channels[0].clone();
			channels[1] = channels[0].clone();
		}
	
		else if (satAltered && valAltered) // Sat, Val
		{
			bitwise_and(channels[2], channels[1], channels[1]);
			channels[2] = channels[1].clone();
			channels[0] = channels[1].clone();
		}
		
		else if (valAltered) // Only Value
		{
			channels[1] = channels[2].clone();
			channels[0] = channels[2].clone();
		}
		
		else if (satAltered) // Only Sat
		{
			channels[2] = channels[1].clone();
			channels[0] = channels[1].clone();
		}
	
		else if (hueAltered) // Only Hue
		{
			channels[1] = channels[0].clone();
			channels[2] = channels[0].clone();
		}
	}

	merge(channels, 3, in);
	delete[] channels;
	cvtColor(in, in, CV_HSV2BGR);
}
