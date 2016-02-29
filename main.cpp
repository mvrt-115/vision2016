#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <unistd.h>
#include <cmath>
#include <thread>

#include "udp_client_server.h"

#include "include/utils/distance.hpp"

#include "include/filters/gaussianBlur.hpp"
#include "include/filters/hsvColorThreshold.hpp"
#include "include/filters/dilateErode.hpp"
#include "include/filters/cannyEdgeDetect.hpp"
#include "include/filters/laplacianSharpen.hpp"
#include "include/filters/houghLines.hpp"
#include "include/filters/mergeFinal.hpp"

const double PI = 3.14159265;
const int LOOPS_PER_SEC = 1; 
const int MICROSEC_TO_SEC = 1000000;

#define DEBUG 0
#define FPS 1
#define JETSON 0

double vertAngle = 0;
double horizAngle = 0;
double fps = 0;
char buff[50];

void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int contoursThresh, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, double& calcDist, double& vertAngle, double& horizAngle);
double calcDistance (cv::Point2f rectPoints[4], double focalLen, int dist, int height);
double calcHorizAngle(int screenWidth, double xDist, std::vector<cv::Point> corners, cv::Point2f& mc);
double calcVertAngle(int height, int dist);
std::vector<cv::Point> corners (std::vector<cv::Point>& pts, int screenWidth, int screenHeight);
void fastHSVThreshold(cv::Mat& image);
void receiveData (udp_client_server::udp_server& server);
void receivePing (udp_client_server::udp_server& server);
void sendData (udp_client_server::udp_client& client);
void drawData(cv::Mat& image, double distance, double horizAngle, double vertAngle);
#if DEBUG
void shapeThreshold(cv::Mat& src, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd);
#else
void shapeThreshold(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd);
#endif

#if DEBUG
void shapeThreshold(cv::Mat& src, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd)
#else
void shapeThreshold(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd)
#endif
{
#if DEBUG
    cv::Mat debug = src.clone();
    char str[50];

	cv::Point2f vert[4];
	for (int i = 0; i < rect.size(); i++)
	{
	    rect[i].points(vert);
		double height = rect[i].size.height;
		double width = rect[i].size.width;
        double rectArea = height * width;
        double contourArea = cv::contourArea(contours[i]);

        for (int v = 0; v < 4; v++)
        {
            sprintf(str, "%i", v);
            cv::putText(debug, str, vert[v], CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 0), 1, 8, false);
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
            //cv::putText(debug, str, cv::Point(contours[i][0]), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
            contours.erase(contours.begin() + i);
			rect.erase(rect.begin() + i);
			i--; //because vector just got smaller
			//std::cerr << "contour erased..." << "\n";
		}
        else
        {
            goalInd = i;
            //std::cerr << "Ratio (" << i << "): " << (height / width) << ", area: " << rectArea << ", area ratio : " << (contourArea / rectArea) << ", angle: " << rect[i].angle << "\n";
            sprintf(str, "+(%i)", i);
            cv::putText(debug, str, cv::Point(contours[i][0]), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 255, 255), 1, 8, false);
        }
        
        cv::imshow("Debug Shape Threshold", debug);
	}
#else
	for (int i = 0; i < rect.size(); i++)
	{
		double height = rect[i].size.height;
		double width = rect[i].size.width;
        double rectArea = height * width;
        double contourArea = cv::contourArea(contours[i]);

        // If any of the following are true, the shape is not the goal
        if (std::abs((height / width) - sideRatio) > sideThreshold || 
            std::abs((contourArea / rectArea) - areaRatio) > areaThreshold || 
            rectArea < minArea || 
            rectArea > maxArea || 
            (width > height && (std::abs(rect[i].angle) > angleThreshold)) || 
            (width < height && (std::abs(std::abs(rect[i].angle) - 90) > angleThreshold)))
		{
            contours.erase(contours.begin() + i);
			rect.erase(rect.begin() + i);
            // Size of vector got smaller
			i--;
		}
        else
        {
            goalInd = i;
        }
	}
#endif

}

void fastHSVThreshold(cv::Mat& image)
{
    cv::cvtColor(image, image, CV_BGR2HSV);
    cv::Mat *channels = new cv::Mat[3];

    cv::inRange(channels[0], cv::Scalar(0), cv::Scalar(180), channels[0]);
    cv::inRange(channels[1], cv::Scalar(0), cv::Scalar(255), channels[1]);
    cv::inRange(channels[2], cv::Scalar(0), cv::Scalar(255), channels[2]);

    cv::bitwise_and(channels[0], channels[1], channels[0]);
    cv::bitwise_and(channels[0], channels[2], channels[0]);

    channels[1] = channels[0].clone();
    channels[2] = channels[0].clone();

    cv::merge(channels, 3, image);
    delete[] channels;
    cv::cvtColor(image, image, CV_HSV2BGR);
}

void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int contoursThresh, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, double& calcDist, double& vertAngle, double& horizAngle)
{
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	// Convert src to gray format
	cv::cvtColor(src, src, CV_BGR2GRAY);
	// Detect edges using Threshold
	cv::threshold(src, src, contoursThresh, 255, cv::THRESH_BINARY);
	cv::findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::cvtColor(src, src, CV_GRAY2BGR);
	src = cv::Mat::zeros(src.size(), CV_8UC3);

#if 0
	// Get the moments
	std::vector<cv::Moments> mu(contours.size());
	for(int i = 0; i < contours.size(); i++)
		mu[i] = cv::moments(contours[0], false);

	//  Get the mass centers
	std::vector<cv::Point2f> mc(contours.size());
	for(int i = 0; i < contours.size(); i++)
		mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
#endif

	// Get the minimally sized bounded rectangles
	std::vector<cv::RotatedRect> minRect( contours.size());
	for(int i = 0; i < contours.size(); i++)
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));

    for (int i = 0; i < contours.size(); i++)
    {
        cv::Moments mu = cv::moments(contours[i], false);
#if DEBUG
        cv::circle(src, cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00), 5, cv::Scalar(255, 255, 0));
#endif
        // Check if the center of mass is outside the tower goal and whether it is far enough
        double dist = cv::pointPolygonTest(contours[i], cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00), true);
#if DEBUG
        std::cerr << "Dist: " << dist << "\n";
#endif
        if (dist > -5 || dist < -15)
        {
#if DEBUG
            std::cerr << "Contour Erased\n";
#endif
            contours.erase(contours.begin() + i);
            minRect.erase(minRect.begin() + i);
			i--; //because vector just got smaller
        }
    }

	cv::Point2f rectPoints[4];
	// Bounded rectangle is the one at the 0th index
	if (minRect.size() > 0)
	{	
        int goalInd = 0;
		int screenWidth = src.cols;
        int screenHeight = src.rows;
#if DEBUG
        shapeThreshold(src, contours, minRect, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, goalInd);
#else
        shapeThreshold(contours, minRect, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, goalInd);
#endif
		minRect[goalInd].points(rectPoints);
        cv::Moments mu = cv::moments(contours[goalInd], false);
        cv::Point2f mc = cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
        std::vector<cv::Point> c = corners(contours[goalInd], screenWidth, screenHeight);

        // Draw the center of the goal and a line from this point to the center
        cv::line(src, cv::Point(src.cols / 2, mc.y), mc, cv::Scalar (255, 0, 255));
        cv::circle(src, mc, 5, cv::Scalar(255, 0, 255));
        // Draw the bottom left and right points of the goal
        cv::circle(src, c[0], 5, cv::Scalar(255, 0, 255));
        cv::circle(src, c[3], 5, cv::Scalar(255, 0, 255));

		calcDist = calcDistance(rectPoints, focalLen, d, h);
		horizAngle = calcHorizAngle(screenWidth, calcDist, c, mc);
		vertAngle = calcVertAngle(h, calcDist);
	}
    
    // Draw the contours
	for(int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255, 255, 0);
		cv::drawContours(src, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
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

std::vector<cv::Point> corners (std::vector<cv::Point>& pts, int screenWidth, int screenHeight)
{
	cv::Point tLeft = cv::Point (0, 0);
	cv::Point tRight = cv::Point (screenWidth, 0);
	cv::Point bLeft = cv::Point (0, screenHeight);
	cv::Point bRight = cv::Point (screenWidth, screenHeight);

	// Initialize to the maximum possible values
	double tLeftD = screenWidth;
	double tRightD = screenWidth;
	double bLeftD = screenWidth;
	double bRightD = screenWidth;

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

double calcVertAngle(int height, int dist)
{
	return ((static_cast<double>(height) / dist) * 180 / PI);
}

double calcHorizAngle(int screenWidth, double xDist, std::vector<cv::Point> corners, cv::Point2f& mc)
{
	double wPixel = distance2D(corners[0].x, corners[3].x);
	double center = screenWidth / 2;
	double distFromCenter = mc.x - center;
	double inchesPerPixel = 20 / wPixel;

	double yDist = distFromCenter * inchesPerPixel;
	double theta = std::asin(yDist / xDist) * 180 / PI;
	return theta;
}

double calcDistance (cv::Point2f rectPoints[4], double focalLen, int dist, int height)
{
	// 20 inches real width
	int wReal = 20;
	int wPixel = 0;
	double theta = 0;
	double d = 0;

	int length = static_cast<int>(distance (rectPoints[0], rectPoints[3]));
	int width = static_cast<int>(distance (rectPoints[0], rectPoints[1]));

	if (length > width)
		wPixel = length;
	else
		wPixel = width;

	d = (wReal * focalLen) / wPixel;
	theta = asin(static_cast<double>(height) / dist) * 180 / PI;

	return d;
}

void drawData(cv::Mat& image, double distance, double horizAngle, double vertAngle)
{
    char str[30];

	sprintf(str, "Distance     = %4.2f", distance);
	cv::putText(image, str, cv::Point(10, 430), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
	sprintf(str, "Horiz Angle  = %4.2f", horizAngle);
	cv::putText(image, str, cv::Point(10, 450), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
	sprintf(str, "Vert Angle   = %4.2f", vertAngle);
	cv::putText(image, str, cv::Point(10, 470), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
}

int main( int argc, char *argv[])
{
	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 10;
	int sigmaY = 10;

	// hsvColorThreshold parameters
	int hMin = 70;
	int hMax = 180;
	int sMin = 0;
	int sMax = 100;
	int vMin = 80;
	int vMax = 100;
	int debugMode = 0;
	// 0 is none, 1 is bitAnd between h, s, and v
	int bitAnd = 1;

	// drawBoundedRects parameters
	double focalLen = 673.40;
	double cameraHeight = 39;
	const double TOWER_HEIGHT = 84;
	int dist = 148;
	//double vertAngle = 0;
	//double horizAngle = 0;
	double calcDist = 0;
	// Tower height is 6 feet 11 inches (to the bottom of the tape) which is 83 inches
	int height = TOWER_HEIGHT - cameraHeight;
	int contoursThresh = 140;

    // Shape threshold parameters
    int s = 145;
    int a = 35;
    int minA = 800;
    int maxA = 25000;
    int sideT = 100;
    int areaT = 20;
    int angleT = 40;
    double sideRatio = (double) s / 100;
    double areaRatio = (double) a / 100;
    double minArea = (double) minA;
    double maxArea = (double) maxA;
    double sideThreshold = (double) sideT / 100;
    double areaThreshold = (double) areaT / 100;
    double angleThreshold = (double) angleT;

#if JETSON
    std::cerr << "Waiting 10 seconds\n";
    usleep(10000000);
#endif

#if FPS
	FILE *pipe_gp = popen("gnuplot", "w");	
	fputs("set terminal png\n", pipe_gp);
	fputs("plot '-' u 1:2 \n", pipe_gp);

	double avg_total = 0;
	double whileclock = 1;
#endif

#if JETSON
	cv::namedWindow("Testing", cv::WINDOW_AUTOSIZE);
#endif

	std::string addr = "10.1.15.6";
	std::string host = "10.1.15.8";
	int port = 5810;

	udp_client_server::udp_client client(addr, port);
	udp_client_server::udp_server server(host, port);

    std::thread listenForPing (receivePing, std::ref(server));
    listenForPing.join();

    std::thread netSend (sendData, std::ref(client));
    netSend.detach();

    std::thread netReceive (receiveData, std::ref(server));
    netReceive.detach();

	cv::VideoCapture camera (0);
	std::cerr << "Opened camera at port 0\n";

    cv::Mat image;

    char kill = ' ';

	//while (whileclock < 200)
	while ((kill != 'q'))
	{
#if FPS
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
#endif
		camera >> image;

		cv::imshow("RGB", image);
		gaussianBlur(image, blur_ksize, sigmaX, sigmaY);
		hsvColorThreshold(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd);
		drawBoundedRects(image, focalLen, dist, height, contoursThresh, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, calcDist, vertAngle, horizAngle);
        drawData(image, dist, horizAngle, vertAngle);
        cv::imshow("Final", image);
#if FPS
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        fps = 1.0 / timeElapsed.count();
		
		fprintf(pipe_gp, "%f %f\n", whileclock, fps);
		avg_total += fps;
		whileclock++;
#endif
		kill = cv:: waitKey(5);
	}
	
#if FPS
	double avg = avg_total / whileclock;
	fprintf(pipe_gp, "%f %f\n", (double)whileclock * 6 / 5, avg);
	fputs("e\n", pipe_gp);
	int x = pclose(pipe_gp);
#endif
	
	return 0;	
}

void sendData (udp_client_server::udp_client& client)
{
    double loopTime = 1.0 / LOOPS_PER_SEC;
    std::string msg;

    while (true)
    {
        // Stop sending data if an agreed symbol is received
        if (buff[0] != '#')
        {
            clock_t start = clock();
            // Check if the angles are not NaN or inf
            if (std::isfinite(horizAngle) && std::isfinite(vertAngle) && std::abs(horizAngle) < 30.0 && vertAngle < 60.0)
            {
                msg = std::to_string(-1.0 * horizAngle) + " " + std::to_string(vertAngle);
                std::cerr << "Sent Data:  " << msg << "\n";
                client.send(msg.c_str(), strlen(msg.c_str()));
            }

            clock_t end = clock();
            double timeElapsed = (double) (end - start) / CLOCKS_PER_SEC;	

            // Pause until loop ends
            if (timeElapsed < loopTime)
            {
                unsigned int microseconds = static_cast<int>((loopTime - timeElapsed) * MICROSEC_TO_SEC);
                //std::cerr << "Loop took " << timeElapsed << " seconds and stalling for " << static_cast<double>(microseconds) / MICROSEC_TO_SEC << " seconds\n";
                usleep(microseconds);
            }
            // Not on time
            else 
            {
                //std::cerr << "Loop took " << timeElapsed << " seconds and new FPS = " << fps << " [ERROR]\n";
            }
        }
    }    
}

void receiveData (udp_client_server::udp_server& server)
{
    int maxBufferSize = 50;
    int maxWaitSec = 1;
    while (true)
    {
        // Receive data from non-blocking server
        server.timed_recv(buff, maxBufferSize, maxWaitSec);
        std::cerr << "Received Data: " << buff << "\n";
    }
}

void receivePing (udp_client_server::udp_server& server)
{
    int maxBufferSize = 1;
    int maxWaitSec = 1;
    char arr [] = "0";
    while (arr[0] == '0')
    {
        // Receive data from non-blocking server
        server.timed_recv(arr, maxBufferSize, maxWaitSec);
    }
    std::cerr << "Received Ping: " << arr << "\n";
}
