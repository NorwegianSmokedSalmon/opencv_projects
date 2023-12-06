#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <sys/io.h>
#include <sys/types.h>
#include <vector>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h> 
#include <opencv2/videoio/legacy/constants_c.h>


using namespace std;
using namespace cv;
string path0 = "../data";
string out_path = "../result";
vector<string> image_address;
vector<Mat> images;
bool Binarization = false;
vector<Mat> output_images;
CvMemStorage* container = cvCreateMemStorage();
CvSeq* contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), container);//存储边缘
CvSize size;//椭圆尺寸
CvPoint center; //椭圆中心
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

//图像预处理的函数,处理成灰度图像
Mat gray_processing(Mat src){
    Mat gray_image;
	if(src.channels()==3){
		cvtColor(src, gray_image, COLOR_RGB2GRAY);
	}
	else{
		gray_image = src.clone();
	}
	return gray_image;
}

//图像二值化处理
Mat binary_processing(Mat src, double Grayscale_threshold){
    Mat binary;
	threshold(src, binary, Grayscale_threshold, 255, THRESH_BINARY);
	return binary;
}

//拟合椭圆，返回原图+拟合的椭圆的Mat图像类型
Mat fitEllipse(Mat src,Mat src_gray)//原图与原图的灰度图
{

	//将 Mat 类型图像转换为 IplImage
	IplImage src_IplImage = cvIplImage(src);
	IplImage* src_IplImage_pointer = &src_IplImage;
	IplImage gray_IplImage = cvIplImage(src_gray);
	IplImage* gray_IplImage_pointer = &gray_IplImage;
	
	//canny边缘检测
	IplImage* canny_image = cvCloneImage(gray_IplImage_pointer);
	cvCanny(gray_IplImage_pointer, canny_image, 50, 350, 3);

	//生成轮廓
	cvFindContours(canny_image, container, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
	
	//输出椭圆曲线的图
	IplImage* out = cvCloneImage(src_IplImage_pointer);

	//画椭圆
	while(contours!=nullptr)
	{
		int num = contours->total; //轮廓上的点数
		//最少需要6个方程才能拟合
		if (num >= 6)
		{ 
			
			CvMat* points = cvCreateMat(1, num, CV_32FC2);
			CvMat points_mat = cvMat(1, num, CV_32SC2, points->data.ptr);
			cvCvtSeqToArray(contours, points->data.ptr, CV_WHOLE_SEQ);
			//将IplImage里的uchar类型数据转换，提高精度
			cvConvert(&points_mat, points); 
			//调用cvFitEllipse2进行椭圆拟合
			CvBox2D box = cvFitEllipse2(points);
			//更新，确定椭圆的中心和尺寸
			center = cvPointFrom32f(box.center);
			size.width = cvRound(box.size.width * 0.5);
			size.height = cvRound(box.size.height * 0.5);
			//画椭圆
			cvEllipse(out, center, size, box.angle, 0, 360, cvScalar(255, 255, 0));
			cvReleaseMat(&points);
		}
		contours = contours->h_next;
	}
	Mat result = cvarrToMat(out);
    return result;
}

int main(){
    //读取图像
    image_address = getFiles(path0);
    for(int i = 0; i < image_address.size();i++){
        cout<<image_address[i]<<endl;
        Mat image = imread(image_address[i]);
        images.push_back(image);
    }
    Mat src1 = images[0];
	Mat src2 = images[1];
	//灰度处理
	Mat src1_gray = gray_processing(src1);
	Mat src2_gray = gray_processing(src2);
	//进行椭圆拟合并在窗口上展示
    Mat out1 = fitEllipse(src1, src1_gray);
	Mat out2 = fitEllipse(src2, src2_gray);
    imshow("result1", out1);
    waitKey();
	imshow("result2", out2);
    waitKey();
	//存储图像
    imwrite(out_path+"/result1.png", out1);
	imwrite(out_path+"/result2.png", out2);
    return 0;
}