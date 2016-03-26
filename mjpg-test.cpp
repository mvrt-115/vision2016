#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <vector>

const int IMAGE_COLS = 640;
const int IMAGE_ROWS = 480;
const int RESIZE_FACTOR = 1;

int main ()
{
    cv::VideoCapture cam (0);
    if (!cam.isOpened())
    {
        std::cerr << "Error: Camera not opened successfully at port 0\n";
        return -1;
    }
    cv::Mat image;
    bool isOutputColored = true;
    int imgSizeX = IMAGE_COLS * RESIZE_FACTOR;
    int imgSizeY = IMAGE_ROWS * RESIZE_FACTOR;
    int fps = 5;
    std::string outFile = "./image/out.mjpeg";
    char key = ' ';

    while (key != 'q')
    {
        cam >> image;
        cv::imshow("Before Writing", image);
        cv::VideoWriter os (outFile.c_str(), CV_FOURCC('M', 'J', 'P', 'G'), fps, cv::Size(imgSizeX, imgSizeY), isOutputColored);
        if (os.isOpened())
        {
            os.write(image);
        }
        else
        {
            std::cerr << "Error: Cannot write to " << outFile << " successfully\n";
            return -1;
        }
        cv::imshow("After Writing", image);
        key = cv::waitKey(5);
    }
    return 0;
}
