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

const int DEBUG = 0;
const int FPS = 0;

const double PI = 3.14159265;
// Measurements in inches
const double TOWER_HEIGHT = 83;
const double CAMERA_HEIGHT = 6;
const double FOCAL_LENGTH = 555;
const int LOOPS_PER_SEC = 1; 
const int MICROSEC_TO_SEC = 1000000;
const int CAMERA_NUM = 0;
const int CALIB_DIST = 186;

double pitch = 0;
double yaw = 0;
double initVelocity = 0;
double fps = 0;

char buff[50];
const std::string TARGET_ADDR = "10.1.15.2";
const std::string HOST_ADDR = "10.1.15.8";
const int UDP_PORT = 5810;
const char START_SIGNAL = '@';
const char STOP_SIGNAL = '#';
const char GET_SIGNAL = 'G';
const char RESUME_SIGNAL = 'R';
const char PAUSE_SIGNAL = 'P';


void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int contoursThresh, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, double& hypotenuse, double& pitch, double& yaw);
double calcHypotenuse (cv::Point2f rectPoints[4], double focalLen, int dist, int height);
double calcYaw(int screenWidth, double xDist, std::vector<cv::Point> corners, cv::Point2f& mc);
double calcPitch(int height, int dist);
double getPitch (int height, int distance);
double getInitialVelocity (int height, double distance, double pitch);
std::vector<cv::Point> corners (std::vector<cv::Point>& pts, int screenWidth, int screenHeight);
void receiveData (udp_client_server::udp_server& server);
void receivePing (udp_client_server::udp_server& server);
void sendData (udp_client_server::udp_client& client);
void drawData(cv::Mat& image, double distance, double yaw, double pitch);
void shapeThreshold(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd);

void shapeThreshold(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::RotatedRect>& rect, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, int& goalInd)
{
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
}

void drawBoundedRects(cv::Mat& src, double focalLen, int d, int h, int contoursThresh, double sideRatio, double areaRatio, double minArea, double maxArea, double sideThreshold, double areaThreshold, double angleThreshold, double& hypotenuse, double& pitch, double& yaw)
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

	// Get the minimally sized bounded rectangles
	std::vector<cv::RotatedRect> minRect( contours.size());
	for(int i = 0; i < contours.size(); i++)
    {
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
    }
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Moments mu = cv::moments(contours[i], false);
        // Check if the center of mass is outside the tower goal and whether it is far enough
        double dist = cv::pointPolygonTest(contours[i], cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00), true);
        if (dist > -5 || dist < -15)
        {
            contours.erase(contours.begin() + i);
            minRect.erase(minRect.begin() + i);
            // Vector got smaller, decrement loop counter
			i--;
        }
    }
	if (minRect.size() > 0)
	{	
	    cv::Point2f rectPoints[4];
        int goalInd = 0;
		int screenWidth = src.cols;
        int screenHeight = src.rows;

        shapeThreshold(contours, minRect, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, goalInd);
		minRect[goalInd].points(rectPoints);
        cv::Moments mu = cv::moments(contours[goalInd], false);
        cv::Point2f mc = cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
        std::vector<cv::Point> c = corners(contours[goalInd], screenWidth, screenHeight);
        if (DEBUG)
        {
            // Draw a line from the center of mass of the goal to the center of the screen
            cv::line(src, cv::Point(src.cols / 2, mc.y), mc, cv::Scalar (255, 0, 255));
            // Draw center of mass of the goal
            cv::circle(src, mc, 5, cv::Scalar(255, 0, 255));
            // Draw the bottom left and right points of the goal
            cv::circle(src, c[0], 5, cv::Scalar(255, 100, 100));
            cv::circle(src, c[3], 5, cv::Scalar(255, 100, 100));
        }
		hypotenuse = calcHypotenuse(rectPoints, focalLen, d, h);
        int distance = std::pow(hypotenuse, 2) - std::pow(h, 2);
		yaw = calcYaw(screenWidth, hypotenuse, c, mc);
		pitch = calcPitch(h, distance);
        //pitch = getPitch(h, distance);
        initVelocity = getInitialVelocity(h, distance, pitch);
        if (DEBUG)
        {
            // Draw the contours
            for(int i = 0; i < contours.size(); i++)
            {
                cv::Scalar color = cv::Scalar(255, 255, 0);
                cv::drawContours(src, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
            }
        }
	}
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

// Calculate the pitch for a straight line of sight to the tower goal
double calcPitch(int height, int dist)
{
	return ((static_cast<double>(height) / dist) * 180 / PI);
}

// Calculate the yaw to turn relative to the current yaw
double calcYaw(int screenWidth, double xDist, std::vector<cv::Point> corners, cv::Point2f& mc)
{
	double wPixel = distance2D(corners[0].x, corners[3].x);
	double center = screenWidth / 2;
	double distFromCenter = mc.x - center;
	double inchesPerPixel = 20 / wPixel;

	double yDist = distFromCenter * inchesPerPixel;
	double theta = std::asin(yDist / xDist) * 180 / PI;
	return theta;
}

// Calculate the hypotenuse length (minimal distance) towards the tower goal
double calcHypotenuse (cv::Point2f rectPoints[4], double focalLen, int dist, int height)
{
	// 20 inches real width
	int wReal = 20;
	int wPixel = 0;
	double theta = 0;
	double d = 0;

	int length = static_cast<int>(distance (rectPoints[0], rectPoints[3]));
	int width = static_cast<int>(distance (rectPoints[0], rectPoints[1]));
	if (length > width)
    {
		wPixel = length;
    }
	else
    {
		wPixel = width;
    }
	d = (wReal * focalLen) / wPixel;
	theta = asin(static_cast<double>(height) / dist) * 180 / PI;
	return d;
}

void drawData(cv::Mat& image, double distance, double yaw, double pitch)
{
    char str[30];

	sprintf(str, "Distance     = %4.2f", distance);
	cv::putText(image, str, cv::Point(10, 430), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
	sprintf(str, "Horiz Angle  = %4.2f", yaw);
	cv::putText(image, str, cv::Point(10, 450), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
	sprintf(str, "Vert Angle   = %4.2f", pitch);
	cv::putText(image, str, cv::Point(10, 470), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.75, cv::Scalar(255, 0, 200), 1, 8, false);
}

// Get the pitch towards the tower goal that will hit the goal at the apex of a parabola
double getPitch (int height, int distance)
{
    return std::atan(2.0 * height / distance);
}

// Get the initial velocity to hit the tower goal at the apex using kinematics
double getInitialVelocity (int height, double distance, double pitch)
{
    const double GRAVITY = 9.81;
    return std::sqrt(2 * GRAVITY * height / std::sin(pitch));
}

int main( int argc, char *argv[])
{
	// gaussianBlur parameters
	int blur_ksize = 7;
	int sigmaX = 25;
	int sigmaY = 25;

	// hsvColorThreshold parameters
	int hMin = 135;
	int hMax = 180;
	int sMin = 0;
	int sMax = 100;
	int vMin = 80;
	int vMax = 100;
	int debugMode = 0;
	// 0 is none, 1 is bitAnd between h, s, and v
	int bitAnd = 1;

	// drawBoundedRects parameters
	double calcDist = 0;
    // Calculate the displacement along the z axis
	int height = TOWER_HEIGHT - CAMERA_HEIGHT;
	int contoursThresh = 140;

    // Shape threshold parameters
    int s = 210;
    int a = 30;
    int minA = 800;
    int maxA = 10000;
    int sideT = 30;
    int areaT = 30;
    int angleT = 20;
    double sideRatio = (double) s / 100;
    double areaRatio = (double) a / 100;
    double minArea = (double) minA;
    double maxArea = (double) maxA;
    double sideThreshold = (double) sideT / 100;
    double areaThreshold = (double) areaT / 100;
    double angleThreshold = (double) angleT;

    FILE *pipe_gp = popen("gnuplot", "w");	
    fputs("set terminal png\n", pipe_gp);
    fputs("plot '-' u 1:2 \n", pipe_gp);

    double avg = 0;
    double fpsTick = 1;

	udp_client_server::udp_client client(TARGET_ADDR, UDP_PORT);
	udp_client_server::udp_server server(HOST_ADDR, UDP_PORT);

    std::cerr << "\nPlease send initial ping to " << HOST_ADDR << "\n\n";

    std::thread listenForPing (receivePing, std::ref(server));
    listenForPing.join();
    std::thread netSend (sendData, std::ref(client));
    netSend.detach();
    std::thread netReceive (receiveData, std::ref(server));
    netReceive.detach();

	cv::VideoCapture camera (CAMERA_NUM);
    if (!camera.isOpened())
    {
        std::cerr << "Error - Could not open camera at port " << CAMERA_NUM << "\n";
        return -1;
    }
	std::cerr << "Opened camera at port " << CAMERA_NUM << "\n";

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    cv::Mat image;
    char kill = ' ';
	while ((kill != 'q'))
	{
        if (FPS)
        {
            start = std::chrono::high_resolution_clock::now();
        }
		camera >> image;
		cv::imshow("RGB", image);
		gaussianBlur(image, blur_ksize, sigmaX, sigmaY);
		hsvColorThreshold(image, hMin, hMax, sMin, sMax, vMin, vMax, debugMode, bitAnd);
		drawBoundedRects(image, FOCAL_LENGTH, CALIB_DIST, height, contoursThresh, sideRatio, areaRatio, minArea, maxArea, sideThreshold, areaThreshold, angleThreshold, calcDist, pitch, yaw);
        drawData(image, CALIB_DIST, yaw, pitch);
        cv::imshow("Final", image);
        if (FPS)
        {
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
            fps = 1.0 / timeElapsed.count();

            fprintf(pipe_gp, "%f %f\n", fpsTick, fps);
            avg += fps;
            fpsTick++;
        }
		kill = cv:: waitKey(5);
	}
    if (FPS)
    {
        avg / fpsTick;
        std::cerr << "Average FPS: " << avg << "\n";
        std::cerr << "Ticks: " << fpsTick << "\n";
        // Clock FPS graphing pipe
        pclose(pipe_gp);
    }
	return 0;	
}

void sendData (udp_client_server::udp_client& client)
{
    double loopTime = 1.0 / LOOPS_PER_SEC;
    std::string msg;

    while (true)
    {
        // Stop sending data if an agreed symbol is received
        if (buff[0] != STOP_SIGNAL)
        {
            clock_t start = clock();
            // Check if the angles are not NaN or inf and within restrictions
            if ((buff[0] == GET_SIGNAL || buff[0] == START_SIGNAL) && std::isfinite(yaw) && std::isfinite(pitch) && std::abs(yaw) < 30.0 && pitch < 90.0)
            {

                msg = std::to_string(yaw) + " " + std::to_string(pitch) + " " + std::to_string(initVelocity);
                std::cerr << "Sent Data:  " << msg << "\n";
                client.send(msg.c_str(), strlen(msg.c_str()));
            }
            else
            {
                std::cerr << "Data not sent, buff = " << buff << "\n";
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
    int maxBufferSize = 15;
    int maxWaitSec = 1;
    while (true)
    {
        // Receive data from non-blocking server
        server.timed_recv(buff, maxBufferSize, maxWaitSec);
        std::cerr << "Received Data: " << buff << "\n";
        if (buff[0] == STOP_SIGNAL)
            std::cerr << "STOP_SIGNAL\n";
        else if (buff[0] == START_SIGNAL)
            std::cerr << "START_SIGNAL\n";
        else if (buff[0] == GET_SIGNAL)
            std::cerr << "GET_SIGNAL\n";
        else if (buff[0] == RESUME_SIGNAL)
            std::cerr << "RESUME_SIGNAL\n";
        else if (buff[0] == PAUSE_SIGNAL)
            std::cerr << "PAUSE_SIGNAL\n";
    }
}

void receivePing (udp_client_server::udp_server& server)
{
    int maxBufferSize = 1;
    int maxWaitSec = 1;
    char buff [] = "0";
    while (buff[0] != START_SIGNAL)
    {
        // Receive data from non-blocking server
        server.timed_recv(buff, maxBufferSize, maxWaitSec);
    }
    std::cerr << "Received start signal: " << buff << "\n";
}
