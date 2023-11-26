#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <sys/io.h>
#include <sys/types.h>
#include <vector>
#include <time.h>
#include "cameracuts.h"
using namespace cv;
Mat Gradient_effect(Mat image1, Mat image2){
   double alpha = 0.5;
   double beta = 1 - alpha;
   double gamma = 0;
   Mat merge;
   merge = addWeighted(image1, alpha, image2, beta, gamma, merge, -1);
   return merge;
}


// int main(){
//     Mat src1 = imread("../data/changyi1.jpg");
//     Mat src2 = imread("../data/changyi2.jpg");
//     Mat merge = Gradient_effect(src1, src2);
//     imshow(merge);
//     return 0;
// }