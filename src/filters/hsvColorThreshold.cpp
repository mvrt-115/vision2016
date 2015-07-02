#include "filters/hsvColorThreshold.hpp"

void hsvColorThreshold(cv::Mat &in, int hMin, int hMax, int sMin, int sMax, int vMin, int vMax, bool DEBUG, bool bitAnd)
{
	bool hueAltered = false;
	bool satAltered = false;
	bool valAltered = false;

	cvtColor(in, in, CV_BGR2HSV);
	cv::Mat *channels = new cv::Mat [3];
	cv::split(in, channels);

	if (hMin != 0 || hMax != 255)
	{
		hueAltered = true;
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
	else if ( (!DEBUG) || (hMin == 0 && hMax == 255) )
	{
		cv::destroyWindow("Hue-Filtered Debug");
	}

	if (sMin != 0 || sMax != 255)
	{
		satAltered = true;
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
	else if ( (!DEBUG) || (sMin == 0 && sMax == 255) )
	{
		cv::destroyWindow("Saturation-Filtered Debug");
	}

	if (vMin != 0 || vMax != 255)
	{
		valAltered = true;
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
	else if ( (!DEBUG) || (vMin == 0 && vMax == 255) )
	{
		cv::destroyWindow("Value-Filtered Debug");	
	}

	if (bitAnd)
	{
		if (hueAltered && valAltered && satAltered) //Sat, Value, Hue
		{
			bitwise_and(channels[0], channels[1], channels[0]);
			bitwise_and(channels[0], channels[2], channels[0]);
			channels[1] = channels[0].clone();
			channels[2] = channels[0].clone();
		}

		else if (valAltered && hueAltered) //Value, Hue
		{
			bitwise_and(channels[0], channels[2], channels[0]);
			channels[2] = channels[0].clone();
			channels[1] = channels[0].clone();
		}

		else if (hueAltered && satAltered) //Sat, Hue
		{
			bitwise_and(channels[0], channels[1], channels[0]);
			channels[2] = channels[0].clone();
			channels[1] = channels[0].clone();
		}
	
		else if (satAltered && valAltered) //Sat, Val
		{
			bitwise_and(channels[2], channels[1], channels[1]);
			channels[2] = channels[1].clone();
			channels[0] = channels[1].clone();
		}
		
		else if (valAltered) //Only Value
		{
			channels[1] = channels[2].clone();
			channels[0] = channels[2].clone();
		}
		
		else if (satAltered) //Only Sat
		{
			channels[2] = channels[1].clone();
			channels[0] = channels[1].clone();
		}
	
		else if (hueAltered) //Only Hue
		{
			channels[1] = channels[0].clone();
			channels[2] = channels[0].clone();
		}
	}

	merge(channels, 3, in);
	delete[] channels;
	cvtColor(in, in, CV_HSV2BGR);
}
