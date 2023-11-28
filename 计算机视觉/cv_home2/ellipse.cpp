#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <sys/io.h>
#include <sys/types.h>
#include <vector>
#include <sys/io.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h> 
#include <opencv2/videoio/legacy/constants_c.h>


using namespace std;
using namespace cv;
string path0 = "../data";
vector<string> image_address;
vector<Mat> images;
vector<vector<Point>> contours;//存储边缘点

// linux 获取图像数据
vector<string> getFiles(string path)
{
    vector<string> address;
    DIR *pDir;
    struct dirent *ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
    }
    while ((ptr = readdir(pDir)) != 0)
    {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            address.push_back(path + "/" + ptr->d_name);
        }
    }
    closedir(pDir);
    return address;
}

//process the image of src_dir, draw ellipses and output the image
// void fitEllipse(string src_dir)
// {
// 	Mat original_mat_Image = imread(src_dir);
// 	Mat gray_mat_Image;
// 	if (original_mat_Image.channels() == 3)
// 	{
// 		//get the grayscale image
// 		cvtColor(original_mat_Image, gray_mat_Image, COLOR_RGB2GRAY);
// 	}
// 	else
// 	{
// 		gray_mat_Image = original_mat_Image.clone();
// 	}
// 	//turn Mat to IplImage
// 	IplImage original_Ipl_Image = cvIplImage(original_mat_Image);
// 	IplImage gray_Ipl_Image = cvIplImage(gray_mat_Image);
// 	IplImage* gray_Ipl_pointer = &gray_Ipl_Image;
// 	IplImage* original_Ipl_pointer = &original_Ipl_Image;

// 	//create contour
// 	CvMemStorage* storage = cvCreateMemStorage();
// 	CvSeq* contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);

// 	//use cvCanny() to find the edges of image and identify it in the graph
// 	IplImage* canny_image = cvCloneImage(gray_Ipl_pointer);
// 	cvCanny(gray_Ipl_pointer, canny_image, 125, 350, 3);

// 	//get contour
// 	cvFindContours(canny_image, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
	
// 	//the image to be drwan with ellipses
// 	IplImage* output = cvCloneImage(original_Ipl_pointer);
// 	//draw ellipses
// 	while(contour)
// 	{
// 		int count = contour->total; //the number of dots in the contour
// 		//avoid too small ellipse
// 		if (count >= 6)
// 		{
// 			CvMat* points_f = cvCreateMat(1, count, CV_32FC2);
// 			CvMat points_i = cvMat(1, count, CV_32SC2, points_f->data.ptr);
// 			cvCvtSeqToArray(contour, points_f->data.ptr, CV_WHOLE_SEQ);
// 			cvConvert(&points_i, points_f);

// 			//ellipse fitting
// 			CvBox2D box = cvFitEllipse2(points_f);

// 			//locate the center and size of ellipse
// 			CvPoint center = cvPointFrom32f(box.center);
// 			CvSize size;
// 			size.width = cvRound(box.size.width * 0.5);
// 			size.height = cvRound(box.size.height * 0.5);
// 			//draw ellipse
// 			cvEllipse(output, center, size, box.angle, 0, 360, cvScalar(0, 255, 0));
// 			cvReleaseMat(&points_f);
// 		}
// 		contour = contour->h_next;
// 	}

// 	//store it into original path
// 	// string filename;
// 	// int i = 0;
// 	// while (i < src_dir.size() && src_dir[i] != '.')
// 	// {
// 	// 	filename += src_dir[i];
// 	// 	i++;
// 	// }
// 	// filename += "_ellipse.png";
// 	// cout << "output: " << filename << endl;
// 	// Mat Mat_output = cvarrToMat(output);
// 	// imwrite(filename, Mat_output);
//     string path = "../result";
    
// }

int main(){
    //load images
    image_address = getFiles(path0);
    for(int i = 0; i < image_address.size();i++){
        cout<<image_address[i]<<endl;
        Mat image = imread(image_address[i]);
        images.push_back(image);
        imshow("image", images[i]);
        waitKey(0);
    }
    // for (int i = 0; i < image_address.size(); i++)
	// 	{
	// 		//process the image of images_addr[i]
	// 		fitEllipse(image_address[i]);
	// 	}
    Mat src1 = images[0];
    Mat src2 = images[1];
    //使用canny检测出边缘
	Mat edge1;
    Mat edge2;
	Canny(src1,edge1,30,70);
    Canny(src2,edge2,30,70);
	imshow("canny边缘1",edge1);
    waitKey(0);
    imshow("canny边缘2",edge2);
    waitKey(0);

    //边缘追踪，没有存储边缘的组织结构
	findContours(edge2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat cimage1 = Mat::zeros(edge2.size(), CV_8UC3);
 
	for(size_t i = 0; i < contours.size(); i++)
	{
		//拟合的点至少为6
		size_t count = contours[i].size();
		if( count < 6 )
			continue;
 
		//椭圆拟合
		RotatedRect box = fitEllipse(contours[i]);
 
		//如果长宽比大于30，则排除，不做拟合
		if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*30 )
			continue;
 
		//画出追踪出的轮廓
		drawContours(cimage1, contours, (int)i, Scalar::all(255), 1, 8);
		
		//画出拟合的椭圆
		ellipse(cimage1, box, Scalar(0,255,0), 1, CV_AA);
	}
	imshow("拟合结果1", cimage1);
 
	waitKey();
    


    return 0;
}