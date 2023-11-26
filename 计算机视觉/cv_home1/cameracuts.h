#ifndef CAMERACUTS_H
#define CAMERACUTS_H
#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <sys/io.h>
#include <sys/types.h>
#include <vector>
#include <time.h>
using namespace cv;



Mat Gradient_effect(Mat image1, Mat image2);



#endif